/**
 * lab4_template.c
 *
 * Template file for CprE 288 lab 4
 *
 * @author Zhao Zhang, Chad Nelson, Zachary Glanz
 * @date 08/14/2016
 */

#include "button.h"
#include "Timer.h"
#include "lcd.h"
#include "cyBot_uart.h"  // Functions for communicating between CyBot and Putty (via UART)
// PuTTy: Baud=115200, 8 data bits, No Flow Control, No Parity, COM1

#warning "Possible unimplemented functions"
#define REPLACEME 0

static void uart_send_str(const char *s)
{
    while (*s)
    {
        cyBot_sendByte(*s++);
    }
}

int main(void)
{
    button_init();
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();
    cyBot_uart_init();
    // Don't forget to initialize the cyBot UART before trying to use it

    // YOUR CODE HERE
    //Checkpoint 1
    lcd_clear();

    uint8_t b = button_getButton();
    uint8_t prev_b = b;

    while (1)
    {

        //Checkpoint 1
        b = button_getButton();
//	    lcd_printf("Button Pressed: %d", b);
//	    timer_waitMillis(1000);
        
        if (prev_b != b)
        {

            if (b == 1)
            {
                uart_send_str("Button 1 is being pressed \r\n");
            }
            else if (b == 2)
            {
                uart_send_str("Button 2 is being pressed \r\n");
            }
            else if (b == 3)
            {
                uart_send_str("Button 3 is being pressed \r\n");
            }
            else if (b == 4)
            {
                uart_send_str("Button 4 is being pressed \r\n");
            }

        }
        prev_b = b;

//	    timer_waitMillis(150);

    }
    return 0;
}
