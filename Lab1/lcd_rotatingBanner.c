/// Simple 'Hello, world' program
/**
 * This program prints "Hello, world" to the LCD screen
 * @author Chad Nelson
 * @date 06/26/2012
 *
 * updated: phjones 9/3/2019
 * Description: Added timer_init call, and including Timer.h
 */

#include "Timer.h"
#include "lcd.h"


int main (void) {

	timer_init(); // Initialize Timer, needed before any LCD screen functions can be called 
	              // and enables time functions (e.g. timer_waitMillis)

	lcd_init();   // Initialize the LCD screen.  This also clears the screen. 

    int i;
    int j;
    char greeting[] = "                    Microcontrollers are lots of fun!";

    while(1) {
        for(i = 0; i < strlen(greeting); i++) {

            lcd_clear();

            for(j = 0; j < 20; j++) {
                char c = greeting[(i + j) % strlen(greeting)];
                lcd_putc(c);
            }


            timer_waitMillis(300);
        }
    }


    
	// NOTE: It is recommended that you use only lcd_init(), lcd_printf(), lcd_putc, and lcd_puts from lcd.h.
       // NOTE: For time functions, see Timer.h

	return 0;
}
