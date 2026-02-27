#include <stdio.h>
#include "lcd.h"
#include "cyBot_Scan.h"
#include "cyBot_uart.h"
#include "Timer.h"
#include "open_interface.h"
#include "CheckpointOne.h"
#include "CheckpointTwo.h"
#include "CheckpointThree.h"

int main(void)
{
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    timer_init();
    lcd_init();
    cyBot_uart_init();
    
    cyBOT_init_Scan(0b0111);
           cyBOT_SERVO_cal();
    lcd_clear();
    lcd_puts("Press m");
    int byte = cyBot_getByte();
    lcd_clear();
    lcd_puts("Scanning");
    if ((char) byte == 'm')
    {
        while (1)
        {
            checkPointThree();
        }
    }
    else
    {
        lcd_puts("Not m");
    }

}

