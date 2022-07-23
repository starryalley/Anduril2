/*
 * aux-leds.h: Aux LED functions for Anduril.
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

#ifndef AUX_LEDS_H
#define AUX_LEDS_H

#if defined(USE_INDICATOR_LED) && defined(TICK_DURING_STANDBY)
void indicator_led_update(uint8_t mode, uint8_t arg);
#endif
#if defined(USE_AUX_RGB_LEDS) && defined(TICK_DURING_STANDBY)
uint8_t setting_rgb_mode_now = 0;
void rgb_led_update(uint8_t mode, uint8_t arg);
void rgb_led_voltage_readout(uint8_t bright);

#define RGB_RED 0
#define RGB_YELLOW 1
#define RGB_GREEN 2
#define RGB_CYAN 3
#define RGB_BLUE 4
#define RGB_PURPLE 5
#define RGB_WHITE 6
#define RGB_DISCO 7
#define RGB_RAINBOW 8
#define RGB_VOLTAGE 9
#define RGB_TEMPERATURE 10
#define RGB_OFF (0<<4)
#define RGB_LOW (1<<4)
#define RGB_HIGH (2<<4)
#define RGB_BLINK (3<<4)
#define RGB_BREATH (4<<4)

/*
 * 0: R
 * 1: RG
 * 2:  G
 * 3:  GB
 * 4:   B
 * 5: R B
 * 6: RGB
 * 7: disco
 * 8: rainbow
 * 9: voltage
 * 10: temperature
 */
const PROGMEM uint8_t rgb_led_colors[] = {
    0b00000001,  // 0: red
    0b00000101,  // 1: yellow
    0b00000100,  // 2: green
    0b00010100,  // 3: cyan
    0b00010000,  // 4: blue
    0b00010001,  // 5: purple
    0b00010101,  // 6: white
};
#define RGB_LED_NUM_COLORS 11
#define RGB_LED_NUM_PATTERNS 4
#ifndef RGB_LED_OFF_DEFAULT
#define RGB_LED_OFF_DEFAULT 0x19  // low, voltage
//#define RGB_LED_OFF_DEFAULT 0x18  // low, rainbow
#endif
#ifndef RGB_LED_LOCKOUT_DEFAULT
#define RGB_LED_LOCKOUT_DEFAULT 0x37  // blinking, disco
#endif
#ifndef RGB_RAINBOW_SPEED
#define RGB_RAINBOW_SPEED 0x0f  // change color every 16 frames
#endif
uint8_t rgb_led_off_mode = RGB_LED_OFF_DEFAULT;
uint8_t rgb_led_lockout_mode = RGB_LED_LOCKOUT_DEFAULT;
#endif

#ifdef USE_INDICATOR_LED
    // bits 4-7 control lockout mode
    // bits 0-3 control "off" mode
    // modes are: 0=off, 1=low, 2=high
    //            (below modes are only enabled if TICK_DURING_STANDBY is defined)
    //            3=blinking, 4=blinking low, 5=blinking high, 6=breathing
    #ifdef INDICATOR_LED_DEFAULT_MODE
    uint8_t indicator_led_mode = INDICATOR_LED_DEFAULT_MODE;
    #else
        #ifdef USE_INDICATOR_LED_WHILE_RAMPING
        //uint8_t indicator_led_mode = (1<<4) + 2;
        uint8_t indicator_led_mode = (2<<4) + 1;
        #else
        uint8_t indicator_led_mode = (4<<4) + 1;
        #endif
    #endif
#endif


#endif
