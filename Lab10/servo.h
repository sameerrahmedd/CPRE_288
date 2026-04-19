#ifndef SERVO_H_
#define SERVO_H_

#include <stdint.h>

void servo_init(void);
void servo_move(uint16_t degrees);
uint32_t servo_get_current_match(void);
uint16_t servo_get_current_degrees(void);
void servo_set_calibration(uint32_t right_match, uint32_t left_match);
uint32_t servo_get_match_from_degrees(uint16_t degrees);
void servo_calibrate(void);
void wait_for_release(void);

#endif
