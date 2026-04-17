#include <stdint.h>
#include <stdio.h>
#include "lcd.h"
#include "timer.h"
#include "servo.h"
#include "REF_tm4c123gh6pm.h"
#include "button.h"
#include "ping.h"


void scan(float distances[], int num_steps) {
    int i;
    int angle;
    int step = 180 / (num_steps - 1);

    for (i = 0; i < num_steps; i++) {
        angle = i * step;
        servo_move(angle);
        
        uint32_t pulse = ping_getPulseWidth();
        distances[i] = ping_getDistance(pulse);
        
        lcd_printf("Angle: %d\nDist: %.2f cm", angle, distances[i]);
    }
}


int main(){
	uint16_t degrees = 90;
	uint8_t ccw_direction = 1;
	char line[21];
	uint8_t btn;
	
	timer_init();
	lcd_init();
	button_init();
	servo_init();
	
	//CP1
	lcd_printf("90 Degrees");
	servo_move(90);
	
	lcd_printf("30 Degrees");
	servo_move(30);
	
	lcd_printf("150 Degrees");
	servo_move(150);
	
	lcd_printf("90 Degrees");
	servo_move(90);
	
	while(1){} //Hold at 90 for checkpoint 1
	
	/*
	//CP2
	servo_move(degrees);
	while(1){
		sprintf(line, "M:%lu D:%s", servo_get_current_match(), ccw_direction ? "CCW" : "CW");
		lcd_printf("%s\nDeg:%u", line, degrees);
		
		btn = button_getButton();
		
		//SW button logic 
		if(btn == 1){
			if(ccw_direction && (degrees < 180)){
				degrees += 1;
			} else {
				if (degrees > 0) degrees -= 1;
			}
			servo_move(degrees);
			wait_for_release();
		} else if(btn == 2){
			if(ccw_direction){
				if (degrees <= 175) degrees += 5;
				else degrees = 180;
			} else {
				if (degrees >= 5) degrees -= 5;
				else degrees = 0;
			}
			servo_move(degrees);
			wait_for_release();
		} else if(btn == 3){
			ccw_direction = !ccw_direction;
			wait_for_release();
		} else if (btn == 4){
			if(ccw_direction){
				degrees = 175;
			} else {
				degrees = 5;
			}
			servo_move(degrees);
			wait_for_release();
		}
		timer_waitMillis(50);
	}
	*/
	
	//CP3
	//servo_calibrate();
	
	//CP4 test with lab simple scan like lab 3
	//float distances[19]; // e.g. 10 degree steps = 19 readings
	//scan(distances, 19);
	//servo_move(90); // return to center
}

