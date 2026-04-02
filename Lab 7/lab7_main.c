/*
 * main.c
 *
 *  Created on: Mar 10, 2026
 *      Author: sam4405
 */

#include <stdio.h>
#include "lcd.h"
#include "cyBot_Scan.h"
#include "uart.h"
#include "Timer.h"
#include "open_interface.h"
#include "lab7_checkpoint1.h"

int main(void)
{

    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    timer_init();
    lcd_init();
    uart_init();
    cyBOT_init_Scan(0b0111);
    // cyBOT_SERVO_cal(); //Only uncomment when calibrating a new CyBot.
    right_calibration_value = 91000; //Change Values Based on CyBot
    left_calibration_value = 1456000; //Change Values Based on CyBot
    lcd_clear();
    lcd_puts("Press m key");
    int byte = uart_receive();



        lcd_clear();

        if ((char) byte == 'm')
        {
            lcd_clear();
            lcd_puts("Got: ");
            lcd_putc((char) byte);
            checkPointThree();
        }
        else
        {
            lcd_puts("Not m");
        }




    return 0;
}


