#include <stdint.h>
#include <stdio.h>
#include "lcd.h"
#include "Timer.h"
#include "servo.h"
#include "REF_tm4c123gh6pm.h"
#include "button.h"

int main(void) {
    timer_init();
    lcd_init();
    button_init();
    servo_init();

    // CP1: Move servo through positions
    lcd_printf("Moving to 90");
    servo_move(90);
    timer_waitMillis(1000);

    lcd_printf("Moving to 30");
    servo_move(30);
    timer_waitMillis(1000);

    lcd_printf("Moving to 150");
    servo_move(150);
    timer_waitMillis(1000);

    lcd_printf("Holding at 90");
    servo_move(90);

    while(1) {} // Hold at 90

    // CP2: Button control
    /*
    uint16_t degrees = 90;
    uint8_t ccw_direction = 1; // 1=CCW, 0=CW
    uint8_t btn;
    char line[21];

    servo_move(degrees);

    while (1) {
        btn = button_getButton();
        sprintf(line, "M:%lu %s", servo_get_current_match(), ccw_direction ? "CCW" : "CW");
        lcd_printf("%s\nDeg:%u", line, degrees);

        if (btn == 1) {
            if (ccw_direction) {
                if (degrees < 180) degrees += 1;
            } else {
                if (degrees > 0) degrees -= 1;
            }
            servo_move(degrees);
            wait_for_release();
        } else if (btn == 2) {
            if (ccw_direction) {
                if (degrees <= 175) degrees += 5;
                else degrees = 180;
            } else {
                if (degrees >= 5) degrees -= 5;
                else degrees = 0;
            }
            servo_move(degrees);
            wait_for_release();
        } else if (btn == 3) {
            ccw_direction = !ccw_direction;
            wait_for_release();
        } else if (btn == 4) {
            degrees = ccw_direction ? 175 : 5;
            servo_move(degrees);
            wait_for_release();
        }

        timer_waitMillis(50);
    }
    */
    
    // CP3: Calibration
    /*
    servo_calibrate();
    while (1) {
        servo_move(0);
        timer_waitMillis(1000);
        servo_move(90);
        timer_waitMillis(1000);
        servo_move(180);
        timer_waitMillis(1000);
    }
    */

    // CP4: Scan with ping sensor
    /*
    ping_init();
    int i;
    int angle;
    float distances[19];
    int num_steps = 19;
    int step = 180 / (num_steps - 1);

    for (i = 0; i < num_steps; i++) {
        angle = i * step;
        servo_move(angle);
        uint32_t pulse = ping_getPulseWidth();
        distances[i] = ping_getDistance(pulse);
        lcd_printf("Ang:%d\nDist:%.1fcm", angle, distances[i]);
    }
    servo_move(90);
    while(1){}
    */
}
