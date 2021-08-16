/*
 * fsm-ramping.h: Ramping functions for SpaghettiMonster.
 * Handles 1- to 4-channel smooth ramping on a single LED.
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

#ifndef FSM_RAMPING_H
#define FSM_RAMPING_H

#ifdef USE_RAMPING

// actual_level: last ramp level set by set_level()
uint8_t actual_level = 0;
#ifdef USE_AUX_RGB_LEDS
// if true (default) reset AUX LED (off) when we set_level(),
//  and button LED will be set according to AUX LED state
// when false, AUX LED won't get set when set_level() is called or
//  button LED won't get set when rgb_led_update() is called.
uint8_t aux_led_reset = 1;
#endif

#ifdef USE_TINT_RAMPING
#ifdef TINT_RAMP_TOGGLE_ONLY
uint8_t tint = 0;
#else
uint8_t tint = 128;
#endif
#define USE_TRIANGLE_WAVE
#endif

#ifdef USE_SET_LEVEL_GRADUALLY
// adjust brightness very smoothly
uint8_t gradual_target;
inline void set_level_gradually(uint8_t lvl);
void gradual_tick();
#endif

// auto-detect the data type for PWM tables
#ifndef PWM_BITS
#define PWM_BITS 8
#define PWM_TOP 255
#endif
#if PWM_BITS <= 8
#define PWM_DATATYPE uint8_t
#define PWM_TOP 255
#define PWM_GET(x,y) pgm_read_byte(x+y)
#else
#define PWM_DATATYPE uint16_t
#ifndef PWM_TOP
#define PWM_TOP 1023  // 10 bits by default
#endif
// pointer plus 2*y bytes
//#define PWM_GET(x,y) pgm_read_word(x+(2*y))
// nope, the compiler was already doing the math correctly
#define PWM_GET(x,y) pgm_read_word(x+y)
#endif

// use UI-defined ramp tables if they exist
#ifdef PWM1_LEVELS
PROGMEM const PWM_DATATYPE pwm1_levels[] = { PWM1_LEVELS };
#endif
#ifdef PWM2_LEVELS
PROGMEM const PWM_DATATYPE pwm2_levels[] = { PWM2_LEVELS };
#endif
#ifdef PWM3_LEVELS
PROGMEM const PWM_DATATYPE pwm3_levels[] = { PWM3_LEVELS };
#endif
#ifdef PWM4_LEVELS
PROGMEM const PWM_DATATYPE pwm4_levels[] = { PWM4_LEVELS };
#endif

// pulse frequency modulation, a.k.a. dynamic PWM
// (different ceiling / frequency at each ramp level)
#ifdef USE_DYN_PWM
PROGMEM const PWM_DATATYPE pwm_tops[] = { PWM_TOPS };
#endif

// default / example ramps
#ifndef PWM1_LEVELS
#if PWM_CHANNELS == 1
  #if RAMP_LENGTH == 50
    // ../../bin/level_calc.py 1 50 7135 3 0.25 980
    PROGMEM const PWM_DATATYPE pwm1_levels[] = { 3,3,3,3,4,4,4,5,5,6,7,8,9,11,12,14,16,18,20,23,25,28,32,35,39,43,47,52,57,62,68,74,80,87,94,102,110,118,127,136,146,156,167,178,189,201,214,227,241,255 };
  #elif RAMP_LENGTH == 75
    // ../../bin/level_calc.py 1 75 7135 3 0.25 980
    PROGMEM const PWM_DATATYPE pwm1_levels[] = { 3,3,3,3,3,3,4,4,4,4,5,5,5,6,6,7,8,8,9,10,11,12,13,14,15,17,18,20,21,23,25,27,29,31,33,36,38,41,44,47,50,53,56,59,63,67,71,75,79,83,88,93,98,103,108,113,119,125,131,137,143,150,157,164,171,178,186,194,202,210,219,227,236,246,255 };
  #elif RAMP_LENGTH == 150
    // ../../bin/level_calc.py 1 150 7135 3 0.25 980
    PROGMEM const PWM_DATATYPE pwm1_levels[] = { 3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,5,5,5,5,5,6,6,6,6,7,7,7,8,8,8,9,9,9,10,10,11,11,12,12,13,13,14,15,15,16,17,17,18,19,19,20,21,22,23,24,24,25,26,27,28,29,31,32,33,34,35,36,38,39,40,42,43,44,46,47,49,50,52,53,55,57,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,89,91,93,96,98,101,103,106,109,111,114,117,120,123,125,128,131,134,138,141,144,147,151,154,157,161,164,168,171,175,179,183,186,190,194,198,202,206,210,215,219,223,228,232,236,241,246,250,255 };
  #endif
#elif PWM_CHANNELS == 2
  #if RAMP_LENGTH == 50
    // ../../bin/level_calc.py 2 50 7135 4 0.33 150 FET 1 10 1500
    PROGMEM const PWM_DATATYPE pwm1_levels[] = { 4,5,6,8,10,13,17,22,28,35,44,54,65,78,93,109,128,149,171,197,224,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0 };
    PROGMEM const PWM_DATATYPE pwm2_levels[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,7,11,15,20,26,31,37,44,51,58,65,73,82,91,100,110,121,132,143,155,168,181,194,209,224,239,255 };
    #define MAX_1x7135 22
  #elif RAMP_LENGTH == 75
    // ../../bin/level_calc.py 2 75 7135 4 0.33 150 FET 1 10 1500
    PROGMEM const PWM_DATATYPE pwm1_levels[] = { 4,4,5,6,7,8,10,12,14,17,20,24,28,32,37,43,49,56,64,72,82,91,102,114,126,139,153,168,184,202,220,239,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0 };
    PROGMEM const PWM_DATATYPE pwm2_levels[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,5,7,10,13,16,19,23,26,30,34,38,42,47,51,56,61,66,72,77,83,89,95,101,108,115,122,129,136,144,152,160,168,177,186,195,204,214,224,234,244,255 };
    #define MAX_1x7135 33
  #elif RAMP_LENGTH == 150
    // ../../bin/level_calc.py 1 65 7135 1 0.8 150
    // ... mixed with this:
    // ../../bin/level_calc.py 2 150 7135 4 0.33 150 FET 1 10 1500
    PROGMEM const PWM_DATATYPE pwm1_levels[] = { 1,1,2,2,3,3,4,4,5,6,7,8,9,10,12,13,14,15,17,19,20,22,24,26,29,31,34,36,39,42,45,48,51,55,59,62,66,70,75,79,84,89,93,99,104,110,115,121,127,134,140,147,154,161,168,176,184,192,200,209,217,226,236,245,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0 };
    PROGMEM const PWM_DATATYPE pwm2_levels[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,3,4,5,7,8,9,11,12,14,15,17,19,20,22,24,25,27,29,31,33,35,37,39,41,43,45,48,50,52,55,57,59,62,64,67,70,72,75,78,81,84,87,90,93,96,99,102,105,109,112,115,119,122,126,129,133,137,141,144,148,152,156,160,165,169,173,177,182,186,191,195,200,205,209,214,219,224,229,234,239,244,250,255 };
    #define MAX_1x7135 65
    #define HALFSPEED_LEVEL 14
    #define QUARTERSPEED_LEVEL 5
  #endif
#elif PWM_CHANNELS == 3
  #if RAMP_LENGTH == 50
    // ../../bin/level_calc.py 3 50 7135 4 0.33 150 7135 4 1 840 FET 1 10 2000
    PROGMEM const PWM_DATATYPE pwm1_levels[] = { 4,5,6,8,11,15,20,26,34,43,54,67,82,99,118,140,165,192,221,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0 };
    PROGMEM const PWM_DATATYPE pwm2_levels[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,17,25,33,42,52,62,73,85,97,111,125,140,157,174,192,210,230,251,255,255,255,255,255,255,255,255,255,255,0 };
    PROGMEM const PWM_DATATYPE pwm3_levels[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,14,34,54,76,98,122,146,172,198,226,255 };
    #define MAX_1x7135 20
    #define MAX_Nx7135 39
  #elif RAMP_LENGTH == 75
    // ../../bin/level_calc.py 3 75 7135 4 0.33 150 7135 4 1 840 FET 1 10 2000
    PROGMEM const PWM_DATATYPE pwm1_levels[] = { 4,4,5,6,7,9,11,14,16,20,24,28,34,40,46,54,62,71,81,92,104,117,130,146,162,179,198,218,239,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0 };
    PROGMEM const PWM_DATATYPE pwm2_levels[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,9,14,18,23,29,34,40,47,53,60,67,75,83,91,99,108,117,127,137,148,158,170,181,193,206,219,232,246,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0 };
    PROGMEM const PWM_DATATYPE pwm3_levels[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,15,28,42,55,70,84,99,115,131,147,164,181,199,217,236,255 };
    #define MAX_1x7135 30
    #define MAX_Nx7135 59
  #elif RAMP_LENGTH == 150
    // ../../bin/level_calc.py 1 65 7135 1 0.8 150
    // ... mixed with this:
    // ../../../bin/level_calc.py 3 150 7135 1 0.33 150 7135 1 1 850 FET 1 10 1500
    PROGMEM const PWM_DATATYPE pwm1_levels[] = { 1,1,2,2,3,3,4,4,5,6,7,8,9,10,12,13,14,15,17,19,20,22,24,26,29,31,34,36,39,42,45,48,51,55,59,62,66,70,75,79,84,89,93,99,104,110,115,121,127,134,140,147,154,161,168,176,184,192,200,209,217,226,236,245,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0 };
    PROGMEM const PWM_DATATYPE pwm2_levels[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,4,6,8,10,13,15,17,19,22,24,26,29,31,34,37,39,42,45,48,51,54,57,60,64,67,70,74,77,81,85,88,92,96,100,104,108,112,116,121,125,130,134,139,143,148,153,158,163,168,173,179,184,189,195,201,206,212,218,224,230,236,243,249,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,0 };
    PROGMEM const PWM_DATATYPE pwm3_levels[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,19,31,43,55,67,79,91,104,117,130,143,157,170,184,198,212,226,240,255 };
    #define MAX_1x7135 65
    #define MAX_Nx7135 130
    #define HALFSPEED_LEVEL 14
    #define QUARTERSPEED_LEVEL 5
  #endif
#elif PWM_CHANNELS == 4
  4-channel PWM not really supported yet, sorry.
#endif
#endif

// RAMP_SIZE / MAX_LVL
#define RAMP_SIZE (sizeof(pwm1_levels)/sizeof(PWM_DATATYPE))
#define MAX_LEVEL RAMP_SIZE

void set_level(uint8_t level);
//void set_level_smooth(uint8_t level);

#endif  // ifdef USE_RAMPING
#endif
