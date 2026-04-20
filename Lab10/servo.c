#include <stdint.h>
#include <stdio.h>
#include "REF_tm4c123gh6pm.h"
#include "servo.h"
#include "Timer.h"
#include "button.h"
#include "lcd.h"

// At 16MHz, 1 count = 62.5ns
// Period = 20ms = 320000 counts (0x04E200)
// Match = width of LOW pulse
// 0 deg   -> 1.0ms HIGH -> 19.0ms LOW -> match = 304000 (0x04A300)
// 90 deg  -> 1.5ms HIGH -> 18.5ms LOW -> match = 296000 (0x048500)
// 180 deg -> 2.0ms HIGH -> 18.0ms LOW -> match = 288000 (0x046500)

#define SERVO_PERIOD 320000

static uint32_t servo_min_match = 304000; // 0 degrees
static uint32_t servo_max_match = 288000; // 180 degrees

static uint16_t current_degrees = 90;
static uint32_t current_match   = 296000;


//Helper Methods


// Write 24-bit match value split across two registers
static void write_match(uint32_t match) {
    TIMER1_TBMATCHR_R = (match & 0xFFFF);
    TIMER1_TBPMR_R    = ((match >> 16) & 0xFF);
}

void wait_for_release(void) {
    while (button_getButton() != 0) {}
    timer_waitMillis(150);
}

void servo_set_calibration(uint32_t right_match, uint32_t left_match) {
    servo_min_match = right_match;
    servo_max_match = left_match;
}

uint16_t servo_get_current_degrees(void) {
    return current_degrees;
}

uint32_t servo_get_current_match(void) {
    return current_match;
}

uint32_t servo_get_match_from_degrees(uint16_t degrees) {
    if (degrees > 180) degrees = 180;
    // Linear interpolation: at 0 deg -> servo_min_match, at 180 deg -> servo_max_match
    return servo_min_match + ((servo_max_match - servo_min_match) * (uint32_t)degrees) / 180;
}

void servo_init(void) {
    // Step 1: Enable clocks
    SYSCTL_RCGCTIMER_R |= 0x02; // Timer1
    SYSCTL_RCGCGPIO_R  |= 0x02; // Port B
    while ((SYSCTL_PRTIMER_R & 0x02) == 0) {}
    while ((SYSCTL_PRGPIO_R  & 0x02) == 0) {}

    // Step 2: Disable Timer1B
    TIMER1_CTL_R &= ~0x0100;

    // Step 3: 16-bit timer mode
    TIMER1_CFG_R = 0x04;

    // Step 4: PWM mode (TBMR=2, TBAMS=1 -> 0x0A)
    TIMER1_TBMR_R = 0x0A;

    // Step 5: Non-inverted output
    TIMER1_CTL_R &= ~0x4000;

    // Step 6: Load period (320000 = 0x04E200)
    TIMER1_TBILR_R = 0xE200; // low 16 bits
    TIMER1_TBPR_R  = 0x04;   // upper 8 bits

    // Step 7: Load starting match for 90 degrees
    current_match = servo_get_match_from_degrees(90);
    write_match(current_match);
    current_degrees = 90;

    // Step 8: Configure PB5 as T1CCP1
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R  &= ~0x00F00000; // clear 4 bits for PB5
    GPIO_PORTB_PCTL_R  |=  0x00700000; // set T1CCP1 (value 7)
    GPIO_PORTB_DEN_R   |= 0x20;
    GPIO_PORTB_DIR_R   |= 0x20;

    // Step 9: Enable Timer1B
    TIMER1_CTL_R |= 0x0100;

    timer_waitMillis(500);
}

void servo_move(uint16_t degrees) {
    uint32_t diff;
    uint32_t wait_ms;

    if (degrees > 180) degrees = 180;

    if (degrees > current_degrees)
        diff = degrees - current_degrees;
    else
        diff = current_degrees - degrees;

    current_match = servo_get_match_from_degrees(degrees);
    write_match(current_match);
    current_degrees = degrees;

    // Wait proportional to distance
    wait_ms = 50 + (diff * 5);
    if (wait_ms > 900) wait_ms = 900;
    timer_waitMillis(wait_ms);
}

void servo_calibrate(void) {
    uint32_t captured_right = servo_min_match;
    uint32_t captured_left  = servo_max_match;
    uint8_t btn;
    uint8_t phase = 0; // 0 = calibrate 0 deg, 1 = calibrate 180 deg
    uint8_t done  = 0;

    current_match = servo_get_match_from_degrees(90);
    write_match(current_match);
    current_degrees = 90;
    timer_waitMillis(500);

    while (!done) {
        btn = button_getButton();

        if (phase == 0) {
            lcd_printf("Cal 0deg\nB1- B2+ B4=SET\nM:%lu", current_match);
        } else {
            lcd_printf("Cal 180deg\nB1- B2+ B4=SET\nM:%lu", current_match);
        }

        if (btn == 1) {
            if (current_match > 270000) {
                current_match -= 200;
                write_match(current_match);
            }
            timer_waitMillis(150);
            while (button_getButton() != 0) {}
        } else if (btn == 2) {
            if (current_match < 320000) {
                current_match += 200;
                write_match(current_match);
            }
            timer_waitMillis(150);
            while (button_getButton() != 0) {}
        } else if (btn == 4) {
            if (phase == 0) {
                captured_right = current_match;
                phase = 1;
            } else {
                captured_left = current_match;
                done = 1;
            }
            timer_waitMillis(150);
            while (button_getButton() != 0) {}
        }

        timer_waitMillis(50);
    }

    servo_set_calibration(captured_right, captured_left);
    lcd_printf("Cal saved!\nR:%lu\nL:%lu", captured_right, captured_left);
    timer_waitMillis(2000);
    servo_move(90);
}
