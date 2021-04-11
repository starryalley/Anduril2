/*
 * candle-mode.c: Candle mode for Anduril.
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

#ifndef CANDLE_MODE_C
#define CANDLE_MODE_C

#include "candle-mode.h"

#ifdef USE_SUNSET_TIMER
#include "sunset-timer.h"
#endif

#define CANDLE_AMPLITUDE_MAX 60
#define CANDLE_AMPLITUDE_MIN 10

#define MAX_CANDLE_LEVEL (RAMP_LENGTH-CANDLE_AMPLITUDE-15)
static uint8_t max_candle_level = MAX_CANDLE_LEVEL;
// these should add up to 100
#define CANDLE_WAVE1_MAXDEPTH 30
#define CANDLE_WAVE2_MAXDEPTH 45
#define CANDLE_WAVE3_MAXDEPTH 25
static uint8_t candle_wave1_depth;
static uint8_t candle_wave2_depth;
static uint8_t candle_wave3_depth;

static inline void update_parameters() {
    max_candle_level = RAMP_LENGTH-candle_amplitude-15;
    candle_wave1_depth = CANDLE_WAVE1_MAXDEPTH * candle_amplitude / 100;
    candle_wave2_depth = CANDLE_WAVE2_MAXDEPTH * candle_amplitude / 100;
    candle_wave3_depth = CANDLE_WAVE3_MAXDEPTH * candle_amplitude / 100;
}

uint8_t candle_mode_state(Event event, uint16_t arg) {
    static int8_t ramp_direction = 1;
    static uint8_t candle_wave1 = 0;
    static uint8_t candle_wave2 = 0;
    static uint8_t candle_wave3 = 0;
    static uint8_t candle_wave2_speed = 0;
    static uint8_t candle_mode_brightness = 24;

    #ifdef USE_SUNSET_TIMER
    // let the candle "burn out" and shut itself off
    // if the user told it to
    // cache this in case it changes when the timer is called
    uint8_t sunset_active = sunset_timer;
    // clock tick
    sunset_timer_state(event, arg);
    // if the timer just expired, shut off
    if (sunset_active  &&  (! sunset_timer)) {
        set_state(off_state, 0);
        return MISCHIEF_MANAGED;
    }
    #endif  // ifdef USE_SUNSET_TIMER


    if (event == EV_enter_state) {
        ramp_direction = 1;
        update_parameters();
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_SUNSET_TIMER
    // 2 clicks: cancel timer
    else if (event == EV_2clicks) {
        // parent state just rotated through strobe/flasher modes,
        // so cancel timer...  in case any time was left over from earlier
        sunset_timer = 0;
        return MISCHIEF_MANAGED;
    }
    #endif  // ifdef USE_SUNSET_TIMER
    // hold: change brightness (brighter)
    else if (event == EV_click1_hold) {
        // ramp away from extremes
        if (! arg) {
            if (candle_mode_brightness >= max_candle_level) { ramp_direction = -1; }
            else if (candle_mode_brightness <= 1) { ramp_direction = 1; }
        }
        // change brightness, but not too far
        candle_mode_brightness += ramp_direction;
        if (candle_mode_brightness < 1) candle_mode_brightness = 1;
        else if (candle_mode_brightness > max_candle_level) candle_mode_brightness = max_candle_level;
        return MISCHIEF_MANAGED;
    }
    // reverse ramp direction on hold release
    else if (event == EV_click1_hold_release) {
        ramp_direction = -ramp_direction;
        return MISCHIEF_MANAGED;
    }
    // click, hold: change brightness (dimmer)
    else if (event == EV_click2_hold) {
        ramp_direction = 1;
        if (candle_mode_brightness > 1)
            candle_mode_brightness --;
        return MISCHIEF_MANAGED;
    }
    // 4C: making candle amplitude smaller (candle in sort of stillness)
    else if (event == EV_4clicks) {
        candle_amplitude -= 5;
        if (candle_amplitude < CANDLE_AMPLITUDE_MIN)
            candle_amplitude = CANDLE_AMPLITUDE_MIN;
        update_parameters();
        save_config();
        return MISCHIEF_MANAGED;
    }
    // 5C: making candle amplitude bigger (candle in the wind)
    else if (event == EV_5clicks) {
        candle_amplitude += 5;
        if (candle_amplitude > CANDLE_AMPLITUDE_MAX)
            candle_amplitude = CANDLE_AMPLITUDE_MAX;
        update_parameters();
        save_config();
        return MISCHIEF_MANAGED;
    }
    // 6C: reset candle amplitude
    else if (event == EV_6clicks) {
        candle_amplitude = CANDLE_AMPLITUDE;
        update_parameters();
        save_config();
        blink_once();
        return MISCHIEF_MANAGED;
    }
    // clock tick: animate candle brightness
    else if (event == EV_tick) {
        // un-reverse after 1 second
        if (arg == TICKS_PER_SECOND) ramp_direction = 1;

        // 3-oscillator synth for a relatively organic pattern
        uint8_t add;
        add = ((triangle_wave(candle_wave1) * candle_wave1_depth) >> 8)
            + ((triangle_wave(candle_wave2) * candle_wave2_depth) >> 8)
            + ((triangle_wave(candle_wave3) * candle_wave3_depth) >> 8);
        uint16_t brightness = candle_mode_brightness + add;

        // self-timer dims the light during the final minute
        #ifdef USE_SUNSET_TIMER
        if (1 == sunset_timer) {
            brightness = brightness
                         * ((TICKS_PER_MINUTE>>5) - (sunset_ticks>>5))
                         / (TICKS_PER_MINUTE>>5);
        }
        #endif  // ifdef USE_SUNSET_TIMER

        set_level(brightness);

        // wave1: slow random LFO
        // TODO: make wave slower and more erratic?
        if ((arg & 1) == 0) candle_wave1 += pseudo_rand() & 1;
        // wave2: medium-speed erratic LFO
        candle_wave2 += candle_wave2_speed;
        // wave3: erratic fast wave
        candle_wave3 += pseudo_rand() % 37;
        // S&H on wave2 frequency to make it more erratic
        if ((pseudo_rand() & 0b00111111) == 0)
            candle_wave2_speed = pseudo_rand() % 13;
        // downward sawtooth on wave2 depth to simulate stabilizing
        if ((candle_wave2_depth > 0) && ((pseudo_rand() & 0b00111111) == 0))
            candle_wave2_depth --;
        // random sawtooth retrigger
        if (pseudo_rand() == 0) {
            // random amplitude
            //candle_wave2_depth = 2 + (pseudo_rand() % ((CANDLE_WAVE2_MAXDEPTH * candle_amplitude / 100) - 2));
            candle_wave2_depth = pseudo_rand() % (CANDLE_WAVE2_MAXDEPTH * candle_amplitude / 100);
            //candle_wave3_depth = 5;
            candle_wave2 = 0;
        }
        // downward sawtooth on wave3 depth to simulate stabilizing
        if ((candle_wave3_depth > 2) && ((pseudo_rand() & 0b00011111) == 0))
            candle_wave3_depth --;
        if ((pseudo_rand() & 0b01111111) == 0)
            // random amplitude
            //candle_wave3_depth = 2 + (pseudo_rand() % ((CANDLE_WAVE3_MAXDEPTH * candle_amplitude / 100) - 2));
            candle_wave3_depth = pseudo_rand() % (CANDLE_WAVE3_MAXDEPTH * candle_amplitude / 100);
        return MISCHIEF_MANAGED;
    }
    return EVENT_NOT_HANDLED;
}


#endif

