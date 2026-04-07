/**
 * @file lab9_main.c
 * @author Dillon Conrad
 */

#include "Timer.h"
#include "lcd.h"
#include "ping.h"

// Uncomment or add any include directives that are needed


int main(void) {
	timer_init(); // Must be called before lcd_init(), which uses timer functions
	lcd_init();
	ping_init();

	while(1)
	{
		//CP 1
		ping_trigger();
		timer_waitMillis(500);
		
		//Cp 2
		/*
		uint32_t pulse_width = ping_getPulseWidth();
		lcd_clear();
		lcd_gotoLine(0);
		lcd_printf("PW:%lu cyc", pulse_width);
		
		lcd_gotoLine(1);
		lcd_printf("OVF: %s", g_ping_overflow ? "YES" : "NO");
		
		timer_waitMillis(500);
		*/
		
		//CP 3
		/*
		uint32_t pWid = ping_getPulseWidth();
		float dist = ping_getDistance(pWid);
		float tMilli = pWid / 16000.0f;
		
		lcd_clear();
		lcd_gotoLine(0);
		lcd_printf("C:%lu T:%.2f", pWid, tMilli);
		
		lcd_gotoLine(1);
		lcd_printf("D:%.1f O:%lu", dist, g_ping_overflow_count);
		
		timer_waitMillis(500);
		*/

	}

}
