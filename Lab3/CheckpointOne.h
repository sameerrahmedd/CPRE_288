/*
 * CheckpointOne.h
 *
 *  Created on: Feb 13, 2026
 *      Author: sam4405
 */

#ifndef CHECKPOINTONE_H
#define CHECKPOINTONE_H

#include <stdio.h>
#include "lcd.h"
#include "cyBot_uart.h"


static void uart_send_str(const char *s);

void checkpointOne_uart_echo(void);

#endif /* CHECKPOINTONE_H_ */
