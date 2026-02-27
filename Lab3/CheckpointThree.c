/*
 * CheckpointThree.c
 *
 *  Created on: Feb 15, 2026
 *      Author: sam4405
 */

#include <stdio.h>
#include <stdbool.h>
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
} ObjectInfo;



static void uart_send_str(const char *s) {
    while(*s) {
        cyBot_sendByte(*s++);
        }
    }




static int find_objects(ObjectInfo obj[], int maxObj) {
    cyBOT_Scan_t scan;
    int objCount = 0;

    bool inObj = false;
    int startAng = 0;
	
	int ang;
    for (ang = 0; ang <= 180; ang += 2) {
        cyBOT_Scan(ang, &scan);

        //Enter Object
        if (!inObj && scan.sound_dist < THRESH_IN) {
            inObj = true;
            startAng = ang;
        }

		// Exit Object	
        else if (inObj && scan.sound_dist > THRESH_OUT) {
            int endAng = ang - 2;
            int width = endAng - startAng;

            if (width >= MIN_WIDTH_DEG && objCount < maxObj) {
                int midAng = (startAng + endAng) / 2;

                cyBOT_Scan(midAng, &scan);

                obj[objCount].id = objCount + 1;
                obj[objCount].startAngle = startAng;
                obj[objCount].endAngle = endAng;
                obj[objCount].midPoint = midAng;
                obj[objCount].radialWidth = width;
                obj[objCount].distCm = scan.sound_dist;

                objCount++;
            }

            inObj = false;
        }
    }

    // for 180 degree object
    if (inObj && objCount < maxObj) {
        int endAng = 180;
        int width = endAng - startAng;

        if (width >= MIN_WIDTH_DEG) {
            int midAng = (startAng + endAng) / 2;

            cyBOT_Scan(midAng, &scan);

            obj[objCount].id = objCount + 1;
            obj[objCount].startAngle = startAng;
            obj[objCount].endAngle = endAng;
            obj[objCount].midPoint = midAng;
            obj[objCount].radialWidth = width;
            obj[objCount].distCm = scan.sound_dist;

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
        if(obj[i].radialWidth < obj[best].radialWidth) {
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
        char line[96];
                sprintf(line,
                "Obj %d: mid=%3d deg, dist=%6.2f cm, width=%3d deg\r\n",
                obj[i].id, obj[i].midPoint, obj[i].distCm, obj[i].radialWidth);
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
