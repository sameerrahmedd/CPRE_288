/**
 * lab5_template.c
 *
 * Template file for CprE 288 Lab 5
 *
 * @author Zhao Zhang, Chad Nelson, Zachary Glanz
 * @date 08/14/2016
 *
 * @author Phillip Jones, updated 6/4/2019
 * @author Diane Rover, updated 2/25/2021, 2/17/2022
 */

#include "button.h"
#include "Timer.h"
#include "lcd.h"

#include "cyBot_uart.h"  // Functions for communicating between CyBot and Putty (via UART1)
                         // PuTTy: Baud=115200, 8 data bits, No Flow Control, No Parity, COM1

#include "cyBot_Scan.h"  // Scan using CyBot servo and sensors


#warning "Possible unimplemented functions"
//#define REPLACEME 0

static void uart_send_str(const char *s)
{
    while (*s)
    {
        cyBot_sendByte(*s++);
    }
}


int main(void) {
    button_init();
    timer_init(); // Must be called before lcd_init(), which uses timer functions
    lcd_init();


    int count = 0;
    char buffer[21];


  // initialize the cyBot UART1 before trying to use it

  // (Uncomment ME for UART init part of lab)
     cyBot_uart_init_clean();  // Clean UART1 initialization, before running your UART1 GPIO init code

    // Complete this code for configuring the GPIO PORTB part of UART1 initialization (your UART1 GPIO init code)
      SYSCTL_RCGCGPIO_R |= 0b000010; // see page 340 for corresponding Ports to Enable.
        while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};
          GPIO_PORTB_DEN_R |= 0b0000011;
          GPIO_PORTB_AFSEL_R |= 0b0000011;
      GPIO_PORTB_PCTL_R &= 0xFFFFFF00;     // Force 0's in the desired locations
      GPIO_PORTB_PCTL_R |= 0x00000011;     // Force 1's in the desired locations
         // Or see the notes for a coding alternative to assign a value to the PCTL field

    // (Uncomment ME for UART init part of lab)
        cyBot_uart_init_last_half();  // Complete the UART device configuration

        // Initialize the scan
//        cyBOT_init_Scan(0b0111);
//        cyBOT_SERVO_cal();
        // Remember servo calibration function and variables from Lab 3

    // YOUR CODE HERE
        lcd_clear();
    while(1)
    {
        //Checkpoint 1
//        char c = cyBot_getByte_blocking();
//        cyBot_sendByte(c);
//        lcd_putc(c);

        //Checkpoint 2
        char c = uart_receive();

       if(c == '\r' || count == 20) {
           buffer[count] = '\0';
           lcd_clear();
           lcd_puts(buffer);
           uart_send_str(buffer);
           uart_send_str("\n");
           count = 0;
       }

       if(count < 20) {
           buffer[count++] = c;

       }


    }

}
