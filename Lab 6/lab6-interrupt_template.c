/**
 * lab6-interrupt_template.c
 *
 * Template file for CprE 288 Lab 6
 *
 * @author Diane Rover, 2/15/2020
 *
 */

#include "Timer.h"
#include "lcd.h"
#include "cyBot_Scan.h"  // For scan sensors
#include "uart-interrupt.h"
#include <stdbool.h>
#include "driverlib/interrupt.h"

// Uncomment or add any include directives that you want to use
// #include "open_interface.h"
// #include "movement.h"
// #include "button.h"

// Your code can use the global variables defined in uart-interrupt.c
// They are declared with the extern qualifier in uart-interrupt.h, which makes the variables visible to this file.

#warning "Possible unimplemented functions"
#define REPLACEME 0

extern volatile char received_byte;
extern volatile int byte_ready;

int main(void)
{
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();
    uart_interrupt_init();
    //cyBOT_init_Scan(0b0111);

    // OPTIONAL
    //assign a value to command_byte if you want to know whether that ASCII code is received
    //note that command_byte is global shared variable read by the ISR
    //for example, try using a tab character as a command from PuTTY

    //cyBOT_Scan_t scan;
    lcd_clear();
    lcd_puts("Ready to Scan");

    while (1)
    {

        if (byte_ready)
        {
            lcd_clear();
            lcd_putc(received_byte);
            byte_ready = 0;
        }

        //first, try leaving this loop empty and see what happens
        //then add code for your application
        // OPTIONAL
        //test and reset command_flag if your ISR is updating it
        //for example, if the flag is 1, do something, like send a message to PuTTY or LCD, or stop a sensor scan, etc.
        //be sure to reset command_flag so you don't keep responding to an old flag

    }
}
