/*
 * fsm-ramping.c: Ramping functions for SpaghettiMonster.
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

#ifndef FSM_RAMPING_C
#define FSM_RAMPING_C

#ifdef USE_RAMPING

void set_level(uint8_t level) {
    #ifdef USE_JUMP_START
    // maybe "jump start" the engine, if it's prone to slow starts
    // (pulse the output high for a moment to wake up the power regulator)
    // (only do this when starting from off and going to a low level)
    if ((! actual_level)
            && level
            && (level < jump_start_level)) {
        set_level(jump_start_level);
        delay_4ms(JUMP_START_TIME/4);
    }
    #endif  // ifdef USE_JUMP_START

    actual_level = level;

    #ifdef USE_SET_LEVEL_GRADUALLY
    gradual_target = level;
    #endif

    #ifdef USE_INDICATOR_LED_WHILE_RAMPING
        #ifdef USE_INDICATOR_LED
        if (! go_to_standby)
            indicator_led((level > 0) + (level > DEFAULT_LEVEL));
        #endif
        //if (level > MAX_1x7135) indicator_led(2);
        //else if (level > 0) indicator_led(1);
        //else if (! go_to_standby) indicator_led(0);
    #else
        #if defined(USE_INDICATOR_LED) || defined(USE_AUX_RGB_LEDS)
        if (! go_to_standby) {
            #ifdef USE_INDICATOR_LED
                indicator_led(0);
            #endif
            #ifdef USE_AUX_RGB_LEDS
                if (aux_led_reset) {
                    rgb_led_set(0);
                    #ifdef USE_BUTTON_LED
                        button_led_set((level > 0) + (level > DEFAULT_LEVEL));
                    #endif
                }
            #endif
        }
        #endif
    #endif

    #ifdef OVERRIDE_SET_LEVEL
        set_level_override(level);
    #else

    #ifdef PWM1_CNT
    static uint8_t prev_level = 0;
    uint8_t api_level = level;
    #endif

    //TCCR0A = PHASE;
    if (level == 0) {
        #if PWM_CHANNELS >= 1
        PWM1_LVL = 0;
        #endif
        #if PWM_CHANNELS >= 2
        PWM2_LVL = 0;
        #endif
        #if PWM_CHANNELS >= 3
        PWM3_LVL = 0;
        #endif
        #if PWM_CHANNELS >= 4
        PWM4_LVL = 0;
        #endif
        // disable the power channel, if relevant
        #ifdef LED_ENABLE_PIN
        LED_ENABLE_PORT &= ~(1 << LED_ENABLE_PIN);
        #endif
        #ifdef LED2_ENABLE_PIN
        LED2_ENABLE_PORT &= ~(1 << LED2_ENABLE_PIN);
        #endif
    } else {
        // enable the power channel, if relevant
        #ifdef LED_ENABLE_PIN
            #ifndef LED_ENABLE_PIN_LEVEL_MIN
            LED_ENABLE_PORT |= (1 << LED_ENABLE_PIN);
            #else
            // only enable during part of the ramp
            if ((level >= LED_ENABLE_PIN_LEVEL_MIN)
                    && (level <= LED_ENABLE_PIN_LEVEL_MAX))
                LED_ENABLE_PORT |= (1 << LED_ENABLE_PIN);
            else  // disable during other parts of the ramp
                LED_ENABLE_PORT &= ~(1 << LED_ENABLE_PIN);
            #endif
        #endif
        #ifdef LED2_ENABLE_PIN
        LED2_ENABLE_PORT |= (1 << LED2_ENABLE_PIN);
        #endif

        // PWM array index = level - 1
        level --;

        #ifdef USE_TINT_RAMPING
        #ifndef TINT_RAMPING_CORRECTION
        #define TINT_RAMPING_CORRECTION 26  // 140% brightness at middle tint
        #endif
        // calculate actual PWM levels based on a single-channel ramp
        // and a global tint value
        uint8_t brightness = PWM_GET(pwm1_levels, level);
        uint8_t warm_PWM, cool_PWM;

        // auto-tint modes
        uint8_t mytint;
        #if 1
        // perceptual by ramp level
        if (tint == 0) { mytint = 255 * (uint16_t)level / RAMP_SIZE; }
        else if (tint == 255) { mytint = 255 - (255 * (uint16_t)level / RAMP_SIZE); }
        #else
        // linear with power level
        //if (tint == 0) { mytint = brightness; }
        //else if (tint == 255) { mytint = 255 - brightness; }
        #endif
        // stretch 1-254 to fit 0-255 range (hits every value except 98 and 198)
        else { mytint = (tint * 100 / 99) - 1; }

        // middle tints sag, so correct for that effect
        uint16_t base_PWM = brightness;
        // correction is only necessary when PWM is fast
        if (level > HALFSPEED_LEVEL) {
            base_PWM = brightness
                     + ((((uint16_t)brightness) * TINT_RAMPING_CORRECTION / 64) * triangle_wave(mytint) / 255);
        }

        cool_PWM = (((uint16_t)mytint * (uint16_t)base_PWM) + 127) / 255;
        warm_PWM = base_PWM - cool_PWM;

        PWM1_LVL = warm_PWM;
        PWM2_LVL = cool_PWM;
        #else  // ifdef USE_TINT_RAMPING

        #if PWM_CHANNELS >= 1
        PWM1_LVL = PWM_GET(pwm1_levels, level);
        #endif
        #if PWM_CHANNELS >= 2
        PWM2_LVL = PWM_GET(pwm2_levels, level);
        #endif
        #if PWM_CHANNELS >= 3
        PWM3_LVL = PWM_GET(pwm3_levels, level);
        #endif
        #if PWM_CHANNELS >= 4
        PWM4_LVL = PWM_GET(pwm4_levels, level);
        #endif

        #endif  // ifdef USE_TINT_RAMPING

        #ifdef USE_DYN_PWM
            uint16_t top = PWM_GET(pwm_tops, level);
            // pulse frequency modulation, a.k.a. dynamic PWM
            PWM1_TOP = top;
            #ifdef PWM1_CNT
            // reset phase if it's out of range
            if (PWM1_CNT >= top) PWM1_CNT = top - 1;
            #endif
            // repeat for other channels if necessary
            #ifdef PMW2_TOP
                #ifdef PWM2_CNT
                if (PWM2_CNT >= top) PWM2_CNT = top - 1;
                #endif
                PWM2_TOP = top;
            #endif
            #ifdef PMW3_TOP
                #ifdef PWM3_CNT
                if (PWM3_CNT >= top) PWM3_CNT = top - 1;
                #endif
                PWM3_TOP = top;
            #endif
        #endif  // ifdef USE_DYN_PWM
        #ifdef PWM1_CNT
            // force reset phase when turning on from zero
            // (because otherwise the initial response is inconsistent)
            if (! prev_level) {
                PWM1_CNT = 0;
                #ifdef PWM2_CNT
                PWM2_CNT = 0;
                #endif
                #ifdef PWM3_CNT
                PWM3_CNT = 0;
                #endif
            }
        #endif
    }
    #ifdef PWM1_CNT
    prev_level = api_level;
    #endif
    #endif  // ifdef OVERRIDE_SET_LEVEL
    #ifdef USE_DYNAMIC_UNDERCLOCKING
    auto_clock_speed();
    #endif
}

#ifdef USE_SET_LEVEL_GRADUALLY
inline void set_level_gradually(uint8_t lvl) {
    gradual_target = lvl;
}

#ifndef OVERRIDE_GRADUAL_TICK
// call this every frame or every few frames to change brightness very smoothly
void gradual_tick() {
    // go by only one ramp level at a time instead of directly to the target
    uint8_t gt = gradual_target;
    if (gt < actual_level) gt = actual_level - 1;
    else if (gt > actual_level) gt = actual_level + 1;

    #ifdef LED_ENABLE_PIN_LEVEL_MIN
    // only enable during part of the ramp
    if ((gt >= LED_ENABLE_PIN_LEVEL_MIN)
            && (gt <= LED_ENABLE_PIN_LEVEL_MAX))
        LED_ENABLE_PORT |= (1 << LED_ENABLE_PIN);
    else  // disable during other parts of the ramp
        LED_ENABLE_PORT &= ~(1 << LED_ENABLE_PIN);
    #endif

    gt --;  // convert 1-based number to 0-based

    PWM_DATATYPE target;

    #if PWM_CHANNELS >= 1
    target = PWM_GET(pwm1_levels, gt);
    if ((gt < actual_level)     // special case for FET-only turbo
            && (PWM1_LVL == 0)  // (bypass adjustment period for first step)
            && (target == PWM_TOP)) PWM1_LVL = PWM_TOP;
    else if (PWM1_LVL < target) PWM1_LVL ++;
    else if (PWM1_LVL > target) PWM1_LVL --;
    #endif
    #if PWM_CHANNELS >= 2
    target = PWM_GET(pwm2_levels, gt);
    if (PWM2_LVL < target) PWM2_LVL ++;
    else if (PWM2_LVL > target) PWM2_LVL --;
    #endif
    #if PWM_CHANNELS >= 3
    target = PWM_GET(pwm3_levels, gt);
    if (PWM3_LVL < target) PWM3_LVL ++;
    else if (PWM3_LVL > target) PWM3_LVL --;
    #endif
    #if PWM_CHANNELS >= 4
    target = PWM_GET(pwm4_levels, gt);
    if (PWM4_LVL < target) PWM4_LVL ++;
    else if (PWM4_LVL > target) PWM4_LVL --;
    #endif

    // did we go far enough to hit the next defined ramp level?
    // if so, update the main ramp level tracking var
    if ((PWM1_LVL == PWM_GET(pwm1_levels, gt))
        #if PWM_CHANNELS >= 2
            && (PWM2_LVL == PWM_GET(pwm2_levels, gt))
        #endif
        #if PWM_CHANNELS >= 3
            && (PWM3_LVL == PWM_GET(pwm3_levels, gt))
        #endif
        #if PWM_CHANNELS >= 4
            && (PWM4_LVL == PWM_GET(pwm4_levels, gt))
        #endif
        )
    {
        actual_level = gt + 1;
    }
    #ifdef USE_DYNAMIC_UNDERCLOCKING
    auto_clock_speed();
    #endif
}
#endif  // ifdef OVERRIDE_GRADUAL_TICK
#endif  // ifdef USE_SET_LEVEL_GRADUALLY

#endif  // ifdef USE_RAMPING
#endif
