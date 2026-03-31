/*
 * CheckpointThree.c
 *
 *  Created on: Feb 15, 2026
 *      Author: sam4405
 */

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "lcd.h"
#include "cyBot_Scan.h"
#include "cyBot_uart.h"

#define THRESH_IN 90.0f
#define THRESH_OUT 100.0f
#define MIN_WIDTH_DEG 4

#define MAX_OBJECTS 10


typedef struct {
    int id;
    int startAngle;
    int endAngle;
    int midPoint;
    int radialWidth;
    float distCm;
    float linearWidth;
} ObjectInfo;



static void uart_send_str(const char *s) {
    while(*s) {
        cyBot_sendByte(*s++);
        }
    }


static int average_ir(int angle) {
    cyBOT_Scan_t scan;
    int sum = 0;
    int i;
    for (i = 0; i < 5; i++) {
        cyBOT_Scan(angle, &scan);
        sum += scan.IR_raw_val;
    }
    return sum / 5;
}




static int find_objects(ObjectInfo obj[], int maxObj) {
    cyBOT_Scan_t scan;
    int objCount = 0;
    bool inObj = false;
    int startAng = 0;

    int ang;
    for (ang = 0; ang <= 180; ang += 2) {
        int ir = average_ir(ang);

        // Enter object — IR sees something close (higher raw val = closer)
        if (!inObj && ir > IR_THRESH_IN) {
            inObj = true;
            startAng = ang;
        }
        // Exit object
        else if (inObj && ir < IR_THRESH_OUT) {
            int endAng = ang - 2;
            int width = endAng - startAng;
            if (width >= MIN_WIDTH_DEG && objCount < maxObj) {
                int midAng = (startAng + endAng) / 2;
                // PING for distance only
                cyBOT_Scan(midAng, &scan);
                obj[objCount].id        = objCount + 1;
                obj[objCount].startAngle = startAng;
                obj[objCount].endAngle   = endAng;
                obj[objCount].midPoint   = midAng;
                obj[objCount].radialWidth = width;
                obj[objCount].distCm     = scan.sound_dist;

                float half_angle_rad = (width / 2.0f) * (M_PI / 180.0f);
                obj[objCount].linearWidth = 2.0f * scan.sound_dist * tanf(half_angle_rad);

                objCount++;
            }
            inObj = false;
        }
    }

    // Handle object still open at 180°
    if (inObj && objCount < maxObj) {
        int endAng = 180;
        int width = endAng - startAng;
        if (width >= MIN_WIDTH_DEG) {
            int midAng = (startAng + endAng) / 2;
            cyBOT_Scan(midAng, &scan);
            obj[objCount].id         = objCount + 1;
            obj[objCount].startAngle = startAng;
            obj[objCount].endAngle   = endAng;
            obj[objCount].midPoint   = midAng;
            obj[objCount].radialWidth = width;
            obj[objCount].distCm     = scan.sound_dist;

            float half_angle_rad = (width / 2.0f) * (M_PI / 180.0f);
            obj[objCount].linearWidth = 2.0f * scan.sound_dist * tanf(half_angle_rad);

            objCount++;
        }
    }
    return objCount;
}

static int smallestWidthObj(ObjectInfo obj[], int count) {
    if(count == 0) {
        return -1;
    }
    int best = 0;
    int i;
    for(i = 0; i < count; i++) {
        if(obj[i].linearWidth < obj[best].linearWidth) {
            best = i;
        }
    }
    return best;
}


void checkPointThree(void) {
    cyBOT_init_Scan(0b0111);
    right_calibration_value = 75250;
    left_calibration_value = 1351000;

    ObjectInfo obj[MAX_OBJECTS];
    int count = find_objects(obj, MAX_OBJECTS);

    uart_send_str("Objects Found: \r\n");

    int i;
    for(i = 0; i < count; i++) {
        char line[120];
                sprintf(line,
                "Obj %d: mid=%3d deg, dist=%6.2f cm, radial=%3d deg, linear=%6.2f cm\r\n",
                obj[i].id, obj[i].midPoint, obj[i].distCm, obj[i].radialWidth, obj[i].linearWidth);
                uart_send_str(line);
            }

            int bestIdx = smallestWidthObj(obj, count);

            if(bestIdx >= 0){
                char msg[64];
                sprintf(msg, "Smallest: Obj %d at %d deg\r\n", obj[bestIdx].id, obj[bestIdx].midPoint);
                uart_send_str(msg);
                
            } else {
                uart_send_str("No objects detected.\r\n");
            }


}
