/**
 * lab6_template.c
 *
 * Template file for CprE 288 Lab 6
 *
 * @author Diane Rover, 2/15/2020
 *
 */

#include "Timer.h"
#include "lcd.h"
#include "cyBot_Scan.h"  // For scan sensors
#include "uart.h"

// Uncomment or add any include directives that are needed
// #include "open_interface.h"
// #include "movement.h"
// #include "button.h"


#warning "Possible unimplemented functions"
//#define REPLACEME 0
//
//
//int main(void) {
//	timer_init(); // Must be called before lcd_init(), which uses timer functions
//	lcd_init();
//	uart_init();
//   cyBOT_init_Scan(0b0111);
//
//	// YOUR CODE HERE
//
//	cyBOT_Scan_t scan;
//
//	while(1)
//	{
//	    int c = uart_receive_nonblocking();
//	    if( c== 'g') {
//	        lcd_clear();
//	        lcd_puts("Scanning...");
//	        int angle = 0;
//	        while(angle<= 180) {
//	            cyBOT_Scan(angle, &scan);
//	            angle+= 2;
//	            int r = uart_receive_nonblocking();
//	            if(r == 's') {
//	                lcd_clear();
//	                lcd_puts("Stopped");
//	                break;
//	            }
//	        }
//	    }
//      // YOUR CODE HERE
//
//
//	}
//
//}
