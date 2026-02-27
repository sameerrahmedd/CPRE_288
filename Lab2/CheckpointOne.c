/*
 * CheckpointOne.c
 *
 *  Created on: Feb 13, 2026
 *      Author: sam4405
 */




#include <stdio.h>
#include "lcd.h"
#include "cyBot_uart.h"

static void uart_send_str(const char *s) {
    while(*s) {
        cyBot_sendByte(*s++);
        }
    }
void checkpointOne_uart_echo(void) {
    lcd_clear();
    lcd_puts("Press a key...");
    int byte = cyBot_getByte();
    lcd_clear();
    lcd_puts("Got: ");
    lcd_putc((char)byte);
    char msg[32];
    sprintf(msg, "Got: %c\r\n", (char)byte);
    uart_send_str(msg);
}

