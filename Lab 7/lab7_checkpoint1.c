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
#include "uart.h"
#include "movement.h"
#include "open_interface.h"


#define IR_THRESH_IN 800
#define IR_THRESH_OUT 740
#define MIN_WIDTH_DEG 4

#define MAX_OBJECTS 10

//Struct to define all variables needed.
typedef struct {
    int id;
    int startAngle;
    int endAngle;
    int midPoint;
    int radialWidth;
    float distCm;
    float linearWidth;
} ObjectInfo;


//Method to send messaged to Putty. To do create new char msg[*correlated bits*], then sprintf(msg, "MESSAGE", variables used in msg), then uart_sendStr();

//Method to average the IR values.
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

//Method to scan and find all objects.
static int find_objects(ObjectInfo obj[], int maxObj) {
    cyBOT_Scan_t scan;
    int objCount = 0;
    bool inObj = false;
    int startAng = 0;

    int ang;
    for (ang = 0; ang <= 180; ang += 2) {
        int ir = average_ir(ang);

        // Enter object, higher raw val = closer
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

    // If Object is 180 degrees
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

//Method that finds the smallest Object to navigate towards.
static int smallestWidthObj(ObjectInfo obj[], int count) {
    //Return -1 if there are no objects.
    if(count == 0) {
        return -1;
    }
    int best = 0;
    int i;

    //Goes through each object and determines which one has the smallest linear width
    // After going through each object whichever object is the smallest turns into the "best" object aka smallest.
    for(i = 0; i < count; i++) {
        if(obj[i].linearWidth < obj[best].linearWidth) {
            best = i;
        }
    }
    return best; //Returns the object number of the smallest object.
}

//Method to navigate towards the smallest object.
void navigate_to_smallest(oi_t *sensor_data) {
    // Scan and find objects
    ObjectInfo obj[MAX_OBJECTS];
    int count = find_objects(obj, MAX_OBJECTS); //Doing count = find_objects(obj, MAX_OBJECTS); at anytime in the code will cause the robot to start scanning for objects.

    //If there are no objects, send to Putty and "quit" out of method.
    if (count == 0) {
        uart_sendStr("No objects found.\r\n");
        return;
    }

    // For each object that is found, print out all neccessary info.
    int i;
    for (i = 0; i < count; i++) {
        char line[120];
        sprintf(line,
            "Obj %d: mid=%3d deg, dist=%6.2f cm, radial=%3d deg, linear=%6.2f cm\r\n",
            obj[i].id, obj[i].midPoint, obj[i].distCm,
            obj[i].radialWidth, obj[i].linearWidth);
        uart_sendStr(line);
    }


    int bestIdx = smallestWidthObj(obj, count); //Best Index aka the Object number that is the smallest object.
    //Send to putty which object it is targeting. (More for double checking purposes).
    char msg[64];
    sprintf(msg, "Targeting Obj %d at %d deg, %.2f cm away\r\n",
            obj[bestIdx].id, obj[bestIdx].midPoint, obj[bestIdx].distCm);
    uart_sendStr(msg);

    // Turn to face target (if object is at degree > than 90 degrees then it should turn left).
    int turn_deg = obj[bestIdx].midPoint - 90;
    if (turn_deg > 0) {
        turn_left(sensor_data, turn_deg);
    } else if (turn_deg < 0) {
        turn_right(sensor_data, -turn_deg);
    }


    double distanceToObj = obj[bestIdx].distCm - 10.0;
    // Drive toward target, stop  10 cm before hitting
    while (distanceToObj > 10.0) {
        //Display how far till target
        char msg[64];
        sprintf(msg, "Distance to target %.2f cm\r\n", distanceToObj);
        uart_sendStr(msg);


        move_forward(sensor_data, 100); // Move forward 100 mm
        distanceToObj -= 10;
        //Updates sensors after moving forward a certain distance.
        oi_update(sensor_data);

        //If updated distance is less than or equal to 10 cm, then stop cybot and quit out.
        if(distanceToObj <= 10.0){
            oi_setWheels(0, 0);
            uart_sendStr("Reached target!\r\n");
            return;

        }

        //If robot gets bumped, detect it, and move around.
        if (sensor_data->bumpLeft || sensor_data->bumpRight) {
            if(sensor_data->bumpLeft){
                uart_sendStr("Bump detected \r\n");
                move_backward(sensor_data, -100);
                turn_right(sensor_data, 45);
                move_forward(sensor_data, 200);
                turn_left(sensor_data, 45);
            } else if (sensor_data->bumpRight){
                uart_sendStr("Bump detected \r\n");
                move_backward(sensor_data, -100);
                turn_left(sensor_data, 45);
                move_forward(sensor_data, 200);
                turn_right(sensor_data, 45);
            }

            // Re-scan to update distance to target
            count = find_objects(obj, MAX_OBJECTS);
            bestIdx = smallestWidthObj(obj, count);
            distanceToObj = obj[bestIdx].distCm - 10.0;
            if (count == 0) {
                uart_sendStr("Lost target \r\n");
                return;
            }
            int retarget_deg = obj[bestIdx].midPoint - 90;
            if (retarget_deg > 0) {
                turn_left(sensor_data, retarget_deg);
            } else if (retarget_deg < 0) {
                turn_right(sensor_data, -retarget_deg);
            }
            continue;
        }
    }



}


void checkPointThree(oi_t *sensor_data) {
    navigate_to_smallest(sensor_data);

}
