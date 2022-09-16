/*
 * aux-leds.c: Aux LED functions for Anduril.
 *
 * Copyright (C) 2017 Selene Scriven
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef AUX_LEDS_C
#define AUX_LEDS_C

#include "aux-leds.h"


#if defined(USE_INDICATOR_LED) && defined(TICK_DURING_STANDBY)
void indicator_led_update(uint8_t mode, uint8_t arg) {
    // turn off aux LEDs when battery is empty
    #ifdef DUAL_VOLTAGE_FLOOR
    if (((voltage < VOLTAGE_LOW) && (voltage > DUAL_VOLTAGE_FLOOR)) || (voltage < DUAL_VOLTAGE_LOW_LOW)) {
    #else
    if (voltage < VOLTAGE_LOW) {
    #endif
        indicator_led(0); 
        return; 
    }

    mode &= 0xf;
    
    // when mode isn't blinky, let's set indicator again
    if (mode <= 2) { indicator_led(mode); return; }

    static const uint8_t seq[] = {0, 1, 2, 1,  0, 0, 0, 0,
                                  0, 0, 1, 0,  0, 0, 0, 0};
    static const uint8_t seq_breath[] = {0, 1, 1, 1,  2, 2, 2, 2,
                                         1, 1, 1, 0,  0, 0, 0, 0,  0, 0, 0, 0};

    uint8_t level = mode;
    switch (mode) {
    case 3:
        // fancy blink, set off/low/high levels here:
        level = seq[arg & 15];
        break;
    case 4:
    case 5:
        // blinking low or high
        level = (arg & 15) ? 0 : mode - 3;
        break;
    case 6:
        level = seq_breath[arg % sizeof(seq_breath)];
        break;
    }
    indicator_led(level);
}
#endif

#if defined(USE_AUX_RGB_LEDS) && defined(TICK_DURING_STANDBY)
uint8_t voltage_to_rgb() {
    static const uint8_t levels[] = {
    // voltage, color
          0, 0, // 0, R
         33, 1, // 1, R+G
         35, 2, // 2,   G
         37, 3, // 3,   G+B
         39, 4, // 4,     B
         41, 5, // 5, R + B
         44, 6, // 6, R+G+B  // skip; looks too similar to G+B
        255, 6, // 7, R+G+B
    };
    uint8_t volts = voltage;
    if (volts < VOLTAGE_LOW) return 0;

    uint8_t i;
    for (i = 0;  volts >= levels[i];  i += 2) {}
    uint8_t color_num = levels[(i - 2) + 1];
    return pgm_read_byte(rgb_led_colors + color_num);
}

#ifdef USE_THERMAL_REGULATION
uint8_t temperature_to_rgb() {
    static const uint8_t temp_levels[] = {
    // temperature in Celsius, color
          0, 5, // 5, R + B =>pink/purple  <=12 C
         12, 4, // 4,     B                (12,16] C
         16, 3, // 3,   G+B =>cyan         (16,20] C
         20, 2, // 2,   G                  (20,25] C
         25, 1, // 1, R+G   =>yellow       (25,28] C
         28, 0, // 0, R                    >28 C
        255, 0, // 0, R
    };
    int16_t temps = temperature;
    if (temps < 0) return 0;

    uint8_t i;
    for (i = 0; temps >= temp_levels[i]; i += 2) {}
    uint8_t color_num = temp_levels[(i - 2) + 1];
    return pgm_read_byte(rgb_led_colors + color_num);
}
#endif

// do fancy stuff with the RGB aux LEDs
// mode: 0bPPPPCCCC where PPPP is the pattern and CCCC is the color
// arg: time slice number
void rgb_led_update(uint8_t mode, uint8_t arg) {
    static uint8_t rainbow = 0;  // track state of rainbow mode
    static uint8_t frame = 0;  // track state of animation mode

    // turn off aux LEDs when battery is empty
    // (but if voltage==0, that means we just booted and don't know yet)
    uint8_t volts = voltage;  // save a few bytes by caching volatile value
    #ifdef DUAL_VOLTAGE_FLOOR
    if ((volts) && (((voltage < VOLTAGE_LOW) && (voltage > DUAL_VOLTAGE_FLOOR)) || (voltage < DUAL_VOLTAGE_LOW_LOW))) {
    #else
    if ((volts) && (volts < VOLTAGE_LOW)) {
    #endif
        rgb_led_set(0);
        #ifdef USE_BUTTON_LED
        button_led_set(0);
        #endif
        return;
    }

    uint8_t pattern = (mode>>4);  // off, low, high, blinking, breathing
    #ifdef USE_BUTTON_LED
    uint8_t button_pattern = pattern; //for button LED
    #endif
    uint8_t color = mode & 0x0f;

    // preview in blinking mode is awkward... use high instead
    if ((! go_to_standby) && (pattern > 2)) { pattern = 2; }


    const uint8_t *colors = rgb_led_colors;
    uint8_t actual_color = 0;
    if (color < 7) {  // normal color
        #ifdef USE_K93_LOCKOUT_KLUDGE
        // FIXME: jank alert: this is dumb
        // this clause does nothing; it just uses up clock cycles
        // because without it, the K9.3's lockout mode fails and returns
        // to "off" after ~5 to 15 seconds when configured for a blinking
        // single color, even though there is no code path from lockout to
        // "off", and it doesn't act like a reboot either (no boot-up blink)
        rainbow = (rainbow + 1 + pseudo_rand() % 5) % 6;
        #endif
        actual_color = pgm_read_byte(colors + color);
    }
    else if (color == 7) {  // disco
        rainbow = (rainbow + 1 + pseudo_rand() % 5) % 6;
        actual_color = pgm_read_byte(colors + rainbow);
    }
    else if (color == 8) {  // rainbow
        uint8_t speed = 0x03;  // awake speed
        if (go_to_standby) speed = RGB_RAINBOW_SPEED;  // asleep speed
        if (0 == (arg & speed)) {
            rainbow = (rainbow + 1) % 6;
        }
        actual_color = pgm_read_byte(colors + rainbow);
    }
    else if (color == 9) {  // voltage
        // show actual voltage while asleep...
        if (go_to_standby) {
            actual_color = voltage_to_rgb();
            // choose a color based on battery voltage
            //if (volts >= 38) actual_color = pgm_read_byte(colors + 4);
            //else if (volts >= 33) actual_color = pgm_read_byte(colors + 2);
            //else actual_color = pgm_read_byte(colors + 0);
        }
        // ... but during preview, cycle colors quickly
        else {
            actual_color = pgm_read_byte(colors + (((arg>>1) % 3) << 1));
        }
    }
    #ifdef USE_THERMAL_REGULATION
    else {   // temperature
        actual_color = temperature_to_rgb();
        if (!go_to_standby) {
            // during preview, flash current temperature's colors quickly
            if (pattern == 1)
                pattern = (arg >> 4) % 2; // special case for pattern 'low': only alternating between off and low
            else
                pattern = (arg >> 2) % 3; // alternating among off, low, high
        }
    }
    #endif

    // uses an odd length to avoid lining up with rainbow loop
    static const uint8_t animation[] = {2, 1, 0, 0,  0, 0, 0, 0,  0,
                                        1, 0, 0, 0,  0, 0, 0, 0,  0, 1};
    static const uint8_t animation_breath[] = {0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 1, 1,  2, 2,
                                               2, 1, 1, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0};
    // pick a brightness from the animation sequence
    #ifdef USE_BUTTON_LED
    uint8_t blink_animation_done = 0;
    #endif
    if (pattern == 3) {
        frame = (frame + 1) % sizeof(animation);
        pattern = animation[frame];
        #ifdef USE_BUTTON_LED
        blink_animation_done = 1;
        #endif
    } else if (pattern == 4) {
        frame = (frame + 1) % sizeof(animation_breath);
        pattern = animation_breath[frame];
        #ifdef USE_BUTTON_LED
        blink_animation_done = 1;
        #endif
    }
    #ifdef USE_BUTTON_LED
    if (button_pattern == 3) {
        if (blink_animation_done) {
            button_pattern = pattern;
        } else {
            frame = (frame + 1) % sizeof(animation);
            button_pattern = animation[frame];
        }
    } else if (button_pattern == 4) {
        if (blink_animation_done) {
            button_pattern = pattern;
        } else {
            frame = (frame + 1) % sizeof(animation_breath);
            button_pattern = animation_breath[frame];
        }
    }
    #endif

    uint8_t result;
    switch (pattern) {
        case 0:  // off
            result = 0;
            break;
        case 1:  // low
            result = actual_color;
            break;
        default:  // high
            result = (actual_color << 1);
            break;
    }
    rgb_led_set(result);

    // separate button LED logic here because the button LED may blink while AUX LED doesn't
    #ifdef USE_BUTTON_LED
    if (aux_led_reset)
        button_led_set((button_pattern > 1) ? 2 : button_pattern);
    #endif
}

void rgb_led_voltage_readout(uint8_t bright) {
    uint8_t color = voltage_to_rgb();
    if (bright) color = color << 1;
    rgb_led_set(color);
}
#endif


#endif

