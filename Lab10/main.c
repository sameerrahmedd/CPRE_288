#include <stdint.h>
#include <stdio.h>
#include "lcd.h"
#include "timer.h"
#include "servo.h"
#include "REF_tm4c123gh6pm.h"
#include "button.h"

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
	/*
	while(1){
	    lcd_printf("Starting...");
	    timer_waitMillis(1000);

	    lcd_printf("90");
	    servo_move(90);
	    timer_waitMillis(1000);
	
	    lcd_printf("30");
	    servo_move(30);
	    timer_waitMillis(1000);

	    lcd_printf("150");
	    servo_move(150);
	    timer_waitMillis(1000);
	
	    lcd_printf("Stay at last 90");
	    servo_move(90);
	    timer_waitMillis(5000);
	}
	*/


	//CP2
	/*
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
	/*
	servo_calibrate();
	while(1){
	    servo_move(45);
	    timer_waitMillis(500);
	    servo_move(90);
	    timer_waitMillis(500);
	    servo_move(135);
	    timer_waitMillis(500);
	}
	*/

	//CP4 test with lab simple scan like lab 3
}
