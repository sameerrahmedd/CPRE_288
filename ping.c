/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "ping.h"
#include "Timer.h"
//Include tm4c macro header file here 

// Global shared variables
// Use extern declarations in the header file

volatile uint32_t g_start_time = 0;
volatile uint32_t g_end_time = 0;
volatile pingState_t g_state = LOW; // State of ping echo pulse
volatile uint8_t g_ping_overflow = 0;
volatile uint32_t g_pulse_width = 0;
volatile uint32_t g_ping_overflow_count = 0;


void ping_init (void){

	// Enable clock to TIMER3 and PORTB
	SYSCTL_RCGCTIMER_R |= 0x08;
	SYSCTL_RCGCGPIO_R |= 0x02;
	
	//Wait until ready
	while((SYSCTL_PRTIMER_R & 0x08) == 0x00){}
	while((SYSCTL_PRGPIO_R & 0x02) == 0x00){}
	
	//Digital enable for PB3
	GPIO_PORTB_DEN_R |= 0x08;
	
	// Initially make PB3 regular GPIO
	GPIO_PORTB_DIR_R |= 0x08;
	GPIO_PORTB_AFSEL_R &= ~0x08;
	GPIO_PORTB_PCTL_R &= 0xFFFF0FFF;
	
	// 1)Disable timer 
	TIMER3_CTL_R &= ~0x0100; //clears enable
	
	// 2) 16-bit timer configuration
	TIMER3_CFG_R = 0x00000004;
	
	// 3) Configure timer 3B for capture, edge-time, count-down
	TIMER3_TBMR_R = 0x00000007;
	
	// 4) Detect both edges
	TIMER3_CTL_R &= ~0x00000C00; //Clears TBEVENT bits
	TIMER3_CTL_R |= 0x00000C00;
	
	// 5) Prescaler
	TIMER3_TBPR_R = 0x000000FF;
	
	// 6) Start Value
	TIMER3_TBILR_R = 0x0000FFFF;
	
	// 7) Enable capture event interrupt for timer 3B
	TIMER3_ICR_R = 0x00000400;
	TIMER3_IMR_R |= 0x00000400;
	
	//Configure PB3 as T3CCP1 alternate function
	GPIO_PORTB_AFSEL_R |= 0x08;
	GPIO_PORTB_PCTL_R &= 0xFFFF0FFF;
	GPIO_PORTB_PCTL_R |= 0x00007000;

    IntRegister(INT_TIMER3B, TIMER3B_Handler);
	IntEnable(INT_TIMER3B);
    IntMasterEnable();

    // Configure and enable the timer
    TIMER3_CTL_R |= 0x00000100;
}

void ping_trigger (void){
    g_state = LOW;
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0x0100;
    TIMER3_IMR_R &= ~0x00000400;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= ~0x08;
	
	//low-high-low trigger pulse on PB3
    GPIO_PORTB_DIR_R |= 0x08;
	timer_waitMicros(0x02);
	
	GPIO_PORTB_DATA_R |= 0x08;
	timer_waitMicros(0x05);
	GPIO_PORTB_DATA_R &= ~0x08;

    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R = 0x00000400;
	
    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_AFSEL_R |= 0x08;
	GPIO_PORTB_PCTL_R &= 0xFFFF0FFF;
    GPIO_PORTB_PCTL_R |= 0x00007000;
	
    TIMER3_IMR_R |= 0x00000400;
    TIMER3_CTL_R |= 0x0100;
}

void TIMER3B_Handler(void){

  if((TIMER3_MIS_R & 0x00000400) == 0x00000400){
	TIMER3_ICR_R = 0x00000400;
	
	if(g_state == LOW){
		g_start_time = TIMER3_TBR_R;
		g_state = HIGH;
	} else if ( g_state == HIGH){
		g_end_time = TIMER3_TBR_R;
		g_state = DONE;
	}
  }
}

float ping_getDistance (uint32_t pWid){

	float time_us = 0.0;
	float distance_cm = 0.0;
	
	time_us = pWid / 16.0f;
	
	distance_cm = (time_us * 0.0343f) / 2.0f;
	
    return distance_cm;

}

uint32_t ping_getPulseWidth(void){
	g_ping_overflow = 0x00;
	g_state = LOW;
	
	ping_trigger();
	while(g_state != DONE){}
	
	if(g_start_time >= g_end_time){
		g_pulse_width = g_start_time - g_end_time;
	} else {
		g_pulse_width = g_start_time + (0x01000000 - g_end_time);
		g_ping_overflow = 0x01;
		g_ping_overflow_count ++;
	}
	
	return g_pulse_width;
}
