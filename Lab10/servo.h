#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>

//Initialize PB5 and Timer 1B PWM
void servo_init(void);

//Helpers
uint32_t servo_get_match_from_degrees(uint16_t degrees);
uint16_t servo_get_current_degrees(void);
uint32_t servo_get_current_match(void);

//Calibration
void servo_set_calibration(uint32_t right_match, uint32_t left_match);

//Moves servo to requested degree (0-180)
void servo_move(uint16_t degrees);

void servo_calibrate(void);

void wait_for_release(void);
#endif

