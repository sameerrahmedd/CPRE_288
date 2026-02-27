/*
 * movement.c
 *
 *  Created on: Feb 3, 2026
 *      Author: sam4405
 */


//#include <stdio.h>
#include "lcd.h"
#include "cyBot_Scan.h"
#include "cyBot_uart.h"
#include "Timer.h"
#include "open_interface.h"
#include "CheckpointOne.h"



double move_forward (oi_t *sensor_data, double distance_mm) {
        // the following code could be put in function move_forward()
        double sum = 0; // distance member in oi_t struct is type double
        oi_setWheels(100,100); //move forward at full speed
        while (sum < distance_mm) {
            oi_update(sensor_data);
            sum += sensor_data -> distance; // use -> notation since pointer
            lcd_printf("%f", sum);
        }
        oi_setWheels(0,0); //stop

        return sum;
}

double move_backward (oi_t *sensor_data, double distance_mm) {
        // the following code could be put in function move_forward()
        double sum = 0; // distance member in oi_t struct is type double
        oi_setWheels(-100,-100); //move backwards at full speed
        while (sum > distance_mm) {
            oi_update(sensor_data);
            sum += sensor_data -> distance; // use -> notation since pointer
            lcd_printf("%f", sum);
        }
        oi_setWheels(0,0); //stop

        return sum;
}

double turn_left(oi_t *sensor_data, double degrees) {
        double totalAngle = 0; //define double Angle variable

       oi_setWheels(50,-50); //start to turn right

       while(totalAngle < degrees) { // while our variable is less than the degrees stated keep updating sensor.

             oi_update(sensor_data);

            totalAngle += sensor_data-> angle;// update totalAngle variable.

      }



        oi_setWheels(0,0); //stop the wheels.

        return totalAngle;
}

double turn_right(oi_t *sensor_data, double degrees) {
        double totalAngle = 0; //define double Angle variable
        degrees = degrees * -1;
       oi_setWheels(-50,50); //start to turn right

       while(totalAngle > degrees) { // while our variable is less than the degrees stated keep updating sensor.

             oi_update(sensor_data);

            totalAngle += sensor_data-> angle;// update totalAngle variable.

      }



        oi_setWheels(0,0); //stop the wheels.

        return totalAngle;
}


    int main(void) {

          oi_t *sensor_data = oi_alloc();
                   oi_init(sensor_data);
                   timer_init();
                   lcd_init();
                   cyBot_uart_init();
                   scan_init_and_calibrate();

                   while(1) {
                       checkpointOne_uart_echo();

                   }
            return 0;


         }








