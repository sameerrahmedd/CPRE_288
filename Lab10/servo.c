#include <stdint.h>
#include <stdio.h>
#include "REF_tm4c123gh6pm.h"
#include "servo.h"
#include "timer.h"
#include "button.h"
#include "lcd.h"

// Period for 20ms at 16MHz, no prescaler: 16,000,000 * 0.020 = 320,000
// Split as: low 16 bits = 0xE200, upper 8 bits = 0x04
#define SERVO_PERIOD 320000

// Match value = width of the LOW pulse
// High pulse = PERIOD - MATCH
// At 16MHz: 1 count = 62.5ns
// 1.0ms high (0 deg)  -> low = 19ms -> match = 304000 = 0x04A300
// 1.5ms high (90 deg) -> low = 18.5ms -> match = 296000 = 0x04 8500
// 2.0ms high (180 deg)-> low = 18ms -> match = 288000 = 0x046500
//
// NOTE: These are starting defaults. Run servo_calibrate() to get
// accurate values for your specific bot.
static uint32_t servo_right_match = 304000; // 0 degrees
static uint32_t servo_left_match  = 288000; // 180 degrees

static uint16_t current_degrees = 90;
static uint32_t current_match   = 296000; // 90 degrees

// Helper to write a 24-bit match value to the match registers
static void write_match(uint32_t match) {
    TIMER1_TBMATCHR_R = (match & 0xFFFF);        // low 16 bits
    TIMER1_TBPMR_R    = ((match >> 16) & 0xFF);  // upper 8 bits
}

// Wait until button is released, then debounce
void wait_for_release(void) {
    while (button_getButton() != 0) {}
    timer_waitMillis(150);
}

void servo_set_calibration(uint32_t right_match, uint32_t left_match) {
    servo_right_match = right_match;
    servo_left_match  = left_match;
}

uint16_t servo_get_current_degrees(void) {
    return current_degrees;
}

uint32_t servo_get_current_match(void) {
    return current_match;
}

// Linear interpolation between 0 and 180 degree match values
uint32_t servo_get_match_from_degrees(uint16_t degrees) {
    if (degrees > 180) degrees = 180;
    return servo_right_match + ((servo_left_match - servo_right_match) * (uint32_t)degrees) / 180;
}

void servo_init(void) {
    // 1. Enable clocks
    SYSCTL_RCGCTIMER_R |= 0x02; // Timer1
    SYSCTL_RCGCGPIO_R  |= 0x02; // Port B

    // 2. Wait until ready
    while ((SYSCTL_PRTIMER_R & 0x02) == 0) {}
    while ((SYSCTL_PRGPIO_R  & 0x02) == 0) {}

    // 3. Disable Timer1B before configuring
    TIMER1_CTL_R &= ~0x0100;

    // 4. Configure Timer1 as two 16-bit timers
    TIMER1_CFG_R = 0x04;

    // 5. Timer1B: periodic + PWM mode (TBMR=2, TBAMS=1 -> 0x0A)
    TIMER1_TBMR_R = 0x0A;

    // 6. Non-inverted PWM output (clear TBPWML)
    TIMER1_CTL_R &= ~0x4000;

    // 7. Load period (320000 = 0x04E200)
    TIMER1_TBILR_R = 0xE200; // low 16
    TIMER1_TBPR_R  = 0x04;   // upper 8

    // 8. Load starting match for 90 degrees
    current_match = servo_get_match_from_degrees(90);
    write_match(current_match);
    current_degrees = 90;

    // 9. Configure PB5 as T1CCP1
    GPIO_PORTB_AFSEL_R |= 0x20;          // enable alternate function on PB5
    GPIO_PORTB_PCTL_R  &= ~0x00F00000;   // clear all 4 bits for PB5
    GPIO_PORTB_PCTL_R  |=  0x00700000;   // set T1CCP1 (value 7)
    GPIO_PORTB_DEN_R   |= 0x20;          // digital enable PB5
    GPIO_PORTB_DIR_R   |= 0x20;          // PB5 as output

    // 10. Enable Timer1B
    TIMER1_CTL_R |= 0x0100;

    // Give servo time to reach center
    timer_waitMillis(500);
}

void servo_move(uint16_t degrees) {
    uint32_t diff;
    uint32_t wait_ms;

    if (degrees > 180) degrees = 180;

    // Calculate how far we're moving for the delay
    if (degrees > current_degrees)
        diff = degrees - current_degrees;
    else
        diff = current_degrees - degrees;

    // Update match register
    current_match = servo_get_match_from_degrees(degrees);
    write_match(current_match);
    current_degrees = degrees;

    // Wait proportional to distance moved (tune if needed)
    wait_ms = 50 + (diff * 4);
    if (wait_ms > 800) wait_ms = 800;
    timer_waitMillis(wait_ms);
}

void servo_calibrate(void) {
    uint32_t captured_right;
    uint32_t captured_left;
    uint8_t btn;
    char line[21];

    servo_move(90);

    // --- Capture 0 degrees ---
    while (1) {
        sprintf(line, "M:%lu", current_match);
        lcd_printf("Cal 0deg\nB1=- B2=+ B4=SET\n%s", line);

        btn = button_getButton();

        if (btn == 1) {
            if (current_match > 270000) {
                current_match -= 100;
                write_match(current_match);
            }
            wait_for_release();
        } else if (btn == 2) {
            if (current_match < 320000) {
                current_match += 100;
                write_match(current_match);
            }
            wait_for_release();
        } else if (btn == 4) {
            captured_right = current_match;
            wait_for_release();
            break;
        }
    }

    // --- Capture 180 degrees ---
    while (1) {
        sprintf(line, "M:%lu", current_match);
        lcd_printf("Cal 180deg\nB1=- B2=+ B4=SET\n%s", line);

        btn = button_getButton();

        if (btn == 1) {
            if (current_match > 270000) {
                current_match -= 100;
                write_match(current_match);
            }
            wait_for_release();
        } else if (btn == 2) {
            if (current_match < 320000) {
                current_match += 100;
                write_match(current_match);
            }
            wait_for_release();
        } else if (btn == 4) {
            captured_left = current_match;
            wait_for_release();
            break;
        }
    }

    servo_set_calibration(captured_right, captured_left);
    lcd_printf("Cal saved!\nR:%lu\nL:%lu", captured_right, captured_left);
    timer_waitMillis(2000);

    servo_move(90);
}
