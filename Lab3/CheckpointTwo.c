/*
 * CheckpointTwo.c
 *
 *  Created on: Feb 15, 2026
 *      Author: sam4405
 */

#include <stdio.h>
#include "lcd.h"
#include "cyBot_Scan.h"
#include "cyBot_uart.h"


static void uart_send_str(const char *s) {
    while(*s) {
        cyBot_sendByte(*s++);
        }
    }
void checkPointTwo(void) {
    cyBOT_init_Scan(0b0111);
    right_calibration_value = 148750;
    left_calibration_value = 1398250;

    cyBOT_Scan_t scan;
    int byte = cyBot_getByte();
    lcd_clear();
    if((char)byte == 'm') {
        lcd_puts("Got: ");
        lcd_putc((char)byte);
        int i;
        cyBOT_Scan(0, &scan);
        for(i = 2; i <= 180; i+=2){
            float oldDistance = scan.sound_dist;
            cyBOT_Scan(i, &scan);
            float newDistance = scan.sound_dist;
              char msg[96];
              sprintf(msg, "Degree: %3d PING: %7.2f   Degree (old): %3d PING (old): %7.2f\r\n", i, newDistance, i-2, oldDistance);
              uart_send_str(msg);
         }

        } else {
        lcd_puts("Not m");
    }

}
