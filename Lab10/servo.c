#include <stdint.h>
#include "REF_tm4c123gh6pm.h"
#include "servo.h"
#include "timer.h"
#include "button.h"
#include "lcd.h"
#include <stdio.h>

#define SERVO_PERIOD 320000

//Default calibration values
static uint32_t servo_right_match = 304000; //1.0 ms high = 0 deg
static uint32_t servo_left_match = 288000; //2.0 ms high = 189 deg

static uint16_t current_degrees = 90;
static uint32_t current_match = 296000; //1.5 ms high = 90 deg

void wait_for_release(){
	while(button_getButton() != 0){}
	timer_waitMillis(150);
}

void servo_set_calibration(uint32_t right_match, uint32_t left_match){
	servo_right_match = right_match;
	servo_left_match = left_match;
}

uint16_t servo_get_current_degrees(){
	return current_degrees;
}

uint32_t servo_get_current_match(){
	return current_match;
}

uint32_t servo_get_match_from_degrees(uint16_t degrees){
	if(degrees > 180){
		degrees = 180;
	}
	
	return servo_right_match + ((servo_left_match - servo_right_match) * degrees) / 180;
}


void servo_init(){
	//Enable clocks for Timer1 and Port B
	SYSCTL_RCGCTIMER_R |= 0x02; //Timer1 
	SYSCTL_RCGCGPIO_R |= 0x02; //Port B
	//Busy waits for timers
	while((SYSCTL_PRTIMER_R & 0x02) == 0x00){}
	while((SYSCTL_PRGPIO_R & 0x02) == 0x00){}
	
	//Disable timer during config
	TIMER1_CTL_R &= ~0x0100; //Clear TBEN
	
	//configure PB5 as T1CCP1
	GPIO_PORTB_AFSEL_R |= 0x20; //PB5 alternate function
	GPIO_PORTB_PCTL_R &= ~0x00700000; //clear PCTL bit's for PB5
	GPIO_PORTB_PCTL_R |= 0x00700000; //PB5=T1CCP1
	GPIO_PORTB_DEN_R |= 0x20; //Digital enable PB5
	GPIO_PORTB_DIR_R |= 0x20; //PB5 Output
	
	//Config Timer1 as 16-bit timer
	TIMER1_CFG_R = 0x00000004;
	
	//Timer1B PWM mode
	TIMER1_TBMR_R = 0x0000000A;
	
	//Non-inverted output
	TIMER1_CTL_R &= ~0x4000; //Clear TBPWML
	
	//Load period = 320000 = 0x04E200
	TIMER1_TBILR_R = 0xE200; //Low 16
	TIMER1_TBPR_R = 0x04; //Upper 8
	
	//Start centered at 90 degrees
	current_match = servo_get_match_from_degrees(90);
	TIMER1_TBMATCHR_R = (current_match & 0xFFFF);//Low 16
	TIMER1_TBPMR_R = ((current_match >> 16) & 0xFF); //Upper 8
	
	current_degrees = 90;
	
	//Enable timer
	TIMER1_CTL_R |= 0x0100; //Set TBEN
	timer_waitMillis(250);
	
}

void servo_calibrate(){
	uint32_t captured_right;
	uint32_t captured_left;
	uint8_t btn;
	char line[21];
	
	//start near centered
	servo_move(90);
	
	//Capture 0 degrees
	while(1){
		sprintf(line, "M:%lu", current_match);
		lcd_printf("Cal 0deg RIGHT\nB1=L B2=R 4=CAP\n%s", line);
		
		btn = button_getButton();
		
		if(btn == 1){
			if(current_match > 280000){
				current_match -= 50;
				TIMER1_TBMATCHR_R = (current_match & 0xFFFF);//Low 16
				TIMER1_TBPMR_R = ((current_match >> 16) & 0xFF); //Upper 8
			}
			wait_for_release();
		} else if(btn == 2){
			if(current_match < 310000){
				current_match += 50;
				TIMER1_TBMATCHR_R = (current_match & 0xFFFF);//Low 16
				TIMER1_TBPMR_R = ((current_match >> 16) & 0xFF); //Upper 8
			}
			wait_for_release();
		} else if(btn == 4){
			captured_right = current_match;
			wait_for_release();
			break;
		}
	}
	
	//Capture 180 degrees
	while(1){
		sprintf(line, "M:%lu", current_match);
		lcd_printf("Cal 180deg Left\nB1=L B2=R 4=CAP\n%s", line);
		
		btn = button_getButton();
		
		if(btn == 1){
			if(current_match > 280000){
				current_match -= 50;
				TIMER1_TBMATCHR_R = (current_match & 0xFFFF);//Low 16
				TIMER1_TBPMR_R = ((current_match >> 16) & 0xFF); //Upper 8
			}
			wait_for_release();
		} else if(btn == 2){
			if(current_match < 310000){
				current_match += 50;
				TIMER1_TBMATCHR_R = (current_match & 0xFFFF);//Low 16
				TIMER1_TBPMR_R = ((current_match >> 16) & 0xFF); //Upper 8
			}
			wait_for_release();
		} else if(btn == 4){
			captured_left = current_match;
			wait_for_release();
			break;
		}
	}
	
	servo_set_calibration(captured_right, captured_left);
	
	lcd_printf("Saved Cal\nR:%lu\nL:%lu", captured_right, captured_left);
	
	//correctly calibrated center 
	servo_move(90);
}
void servo_move(uint16_t degrees){
	uint32_t diff;
	uint32_t wait_ms;
	
	if(degrees > 180){
		degrees = 180;
	}
	
	//How far degree move calculation
	if(degrees > current_degrees){
		diff = degrees - current_degrees;
	}else {
		diff = current_degrees - degrees;
	}
	
	current_match = servo_get_match_from_degrees(degrees);
	
	TIMER1_TBMATCHR_R = (current_match & 0xFFFF);//Low 16
	TIMER1_TBPMR_R = ((current_match >> 16) & 0xFF); //Upper 8
	
	current_degrees = degrees;
	
	//Movement delay, tune if needed
	wait_ms = 40 + (diff * 3);
	if(wait_ms > 700){
		wait_ms = 700;
	}
	
	timer_waitMillis(wait_ms);
}