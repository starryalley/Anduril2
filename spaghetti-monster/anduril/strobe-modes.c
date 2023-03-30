/*
 * strobe-modes.c: Strobe modes for Anduril.
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

#ifndef STROBE_MODES_C
#define STROBE_MODES_C

#include "strobe-modes.h"

#ifdef USE_STROBE_STATE

#ifdef USE_TINT_RAMPING
// for accessing tint
#include "fsm-ramping.h"
// the tint will be changed in some cases here. This is for restoring the original tint
uint8_t saved_tint = 128;
// when user is about to exit the strobe mode, let's stop updating tint after restored saved tint
uint8_t tint_locked = 0;
#endif

#ifdef USE_TINT_RAMPING
uint8_t past_edge_counter = 0;
#endif

// enable static PWM
static inline void set_static_pwm() {
    #ifdef USE_DYN_PWM
    use_static_pwm = 1;
    #endif
}

// restore tint to the original tint when entering strobe mode
static inline void restore_tint() {
    #ifdef USE_TINT_RAMPING
    tint = saved_tint;
    tint_locked = 1;
    #endif
}

// unset static PWM and restore tint
static inline void undo_global_setting() {
    #ifdef USE_DYN_PWM
    use_static_pwm = 0;
    #elif defined(PWM1_TOP)
    // when not using USE_DYN_PWM, we must revert the TOP back to default
    PWM1_TOP = PWM_TOP;
    #endif
    restore_tint();
}

// for some strobe modes we need to enable/disable some global settings:
//  tint or dynamic PWM
static void strobe_global_setup(strobe_mode_te type) {
    switch(type) {
    #ifdef USE_CANDLE_MODE
    case candle_mode_e:
        set_static_pwm();
        #if defined(PWM1_TOP) && defined(PWM_TOP_CANDLE)
        PWM1_TOP = PWM_TOP_CANDLE;
        #endif
        restore_tint();
        break;
    #endif

    #ifdef USE_FIREWORK_MODE
    case firework_mode_e:
        set_static_pwm();
        #if defined(PWM1_TOP) && defined(PWM_TOP_FIREWORK)
        PWM1_TOP = PWM_TOP_FIREWORK;
        #endif
        #ifdef USE_TINT_RAMPING
        tint_locked = 0;
        #endif
        break;
    #endif

    #ifdef USE_TINT_RAMPING
    case tint_alternating_strobe_e:
    case tint_smooth_ramp_e:
        tint_locked = 0;
        break;
    #endif

    default:
        undo_global_setting();
    }
}

uint8_t strobe_state(Event event, uint16_t arg) {
    static int8_t ramp_direction = 1;

    // 'st' reduces ROM size slightly
    strobe_mode_te st = strobe_type;

    #ifdef USE_MOMENTARY_MODE
    momentary_mode = 1;  // 0 = ramping, 1 = strobes
    #endif

    #ifdef USE_CANDLE_MODE
    // pass all events to candle mode, when it's active
    // (the code is in its own pseudo-state to keep things cleaner)
    if (st == candle_mode_e) {
        candle_mode_state(event, arg);
    } else {
        #if defined(USE_AUX_RGB_LEDS) || defined(USE_INDICATOR_LED)
        aux_led_reset = 1;
        #endif
    }
    #endif
    // use static PWM in firework mode
    #if defined(USE_FIREWORK_MODE) && defined(PWM_TOP_FIREWORK)
    if (st == firework_mode_e && event == EV_enter_state) {
        strobe_global_setup(st);
    }
    #endif

    if (0) {}  // placeholder
    // init anything which needs to be initialized
    else if (event == EV_enter_state) {
        ramp_direction = 1;
        #ifdef USE_TINT_RAMPING
        tint_locked = 0;
        saved_tint = tint;
        #endif
        return MISCHIEF_MANAGED;
    }
    // 1 click: off
    else if (event == EV_1click) {
        set_state(off_state, 0);
        #if defined(USE_AUX_RGB_LEDS) || defined(USE_INDICATOR_LED)
        aux_led_reset = 1;
        #endif
        undo_global_setting();
        return MISCHIEF_MANAGED;
    }
    // 2 clicks: rotate through strobe/flasher modes
    else if (event == EV_2clicks) {
        strobe_type = (st + 1) % NUM_STROBES;
        save_config();
        strobe_global_setup(strobe_type);
        return MISCHIEF_MANAGED;
    }
    // 3 clicks: rotate back through strobe/flasher modes
    else if (event == EV_3clicks) {
        strobe_type = (st - 1 + NUM_STROBES) % NUM_STROBES;
        save_config();
        strobe_global_setup(strobe_type);
        return MISCHIEF_MANAGED;
    }
    // hold: change speed (go faster)
    //       or change brightness (brighter)
    else if (event == EV_click1_hold) {
        if (0) {}  // placeholder

        // party / tactical strobe faster
        #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        #ifdef USE_TACTICAL_STROBE_MODE
        else if (st <= tactical_strobe_e) {
        #else
        else if (st == party_strobe_e) {
        #endif
            if ((arg & 1) == 0) {
                uint8_t d = strobe_delays[st];
                d -= ramp_direction;
                if (d < 8) d = 8;
                else if (d > 254) d = 254;
                strobe_delays[st] = d;
            }
        }
        #endif

        // lightning has no adjustments
        //else if (st == lightning_storm_e) {}

        // biking mode brighter
        #ifdef USE_BIKE_FLASHER_MODE
        else if (st == bike_flasher_e) {
            bike_flasher_brightness += ramp_direction;
            if (bike_flasher_brightness < 2) bike_flasher_brightness = 2;
            else if (bike_flasher_brightness > MAX_BIKING_LEVEL) bike_flasher_brightness = MAX_BIKING_LEVEL;
            set_level(bike_flasher_brightness);
        }
        #endif

        #ifdef USE_TINT_RAMPING
        else if (st == tint_alternating_strobe_e) {
            tint_alt_brightness += ramp_direction;
            if (tint_alt_brightness < 2) tint_alt_brightness = 2;
            else if (tint_alt_brightness > (MAX_LEVEL-20)) tint_alt_brightness = (MAX_LEVEL-20);
            set_level(tint_alt_brightness);
        }
        else if (st == tint_smooth_ramp_e) {
            if (! arg) {
                past_edge_counter = 0;  // doesn't start until user hits the edge
            }
            if ((tint_smooth_brightness > 2) && (tint_smooth_brightness < (RAMP_SIZE-20))){ //don't go to 1 (unreliable) or over the single channel max
                tint_smooth_brightness += ramp_direction;
            }
            else if ((tint_smooth_brightness == (MAX_LEVEL-20) && (past_edge_counter == 0))){ set_level(RAMP_SIZE-20); nice_delay_ms(20); blip(); nice_delay_ms(20); past_edge_counter++; } ///top of single channel, blink and start the counter
              if (past_edge_counter < 255) { past_edge_counter++; }
              if (past_edge_counter == 128) { //blink again to indicate start of both channels
                  blip();
              }
              // if the user kept pressing long enough, go to both chanels always on
              if ((past_edge_counter > 128) && ((arg % 16) == 0)) { //button was held past the bump, increment every 16 frames since we don't have as far to go
                if (tint_smooth_brightness < (MAX_LEVEL-1)) { tint_smooth_brightness += ramp_direction; } //don't go up to 150/150 which would just be a solid colour
                //blip();
              }
              set_level(tint_smooth_brightness);
        }
        #endif

        return MISCHIEF_MANAGED;
    }
    // reverse ramp direction on hold release
    // ... and save new strobe settings
    else if (event == EV_click1_hold_release) {
        ramp_direction = -ramp_direction;
        save_config();
        return MISCHIEF_MANAGED;
    }
    // click, hold: change speed (go slower)
    //       or change brightness (dimmer)
    else if (event == EV_click2_hold) {
        ramp_direction = 1;

        if (0) {}  // placeholder

        // party / tactical strobe slower
        #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        #ifdef USE_TACTICAL_STROBE_MODE
        else if (st <= tactical_strobe_e) {
        #else
        else if (st == party_strobe_e) {
        #endif
            if ((arg & 1) == 0) {
                if (strobe_delays[st] < 255) strobe_delays[st] ++;
            }
        }
        #endif

        // lightning has no adjustments
        //else if (st == lightning_storm_e) {}

        // biking mode dimmer
        #ifdef USE_BIKE_FLASHER_MODE
        else if (st == bike_flasher_e) {
            if (bike_flasher_brightness > 2)
                bike_flasher_brightness --;
            set_level(bike_flasher_brightness);
        }
        #endif

        #ifdef USE_TINT_RAMPING
        else if (st == tint_alternating_strobe_e) {
            if (tint_alt_brightness > 2)
                tint_alt_brightness--;
            set_level(tint_alt_brightness);
        }
        else if (st == tint_smooth_ramp_e) {
            if (tint_smooth_brightness > 2)
                tint_smooth_brightness--;
            set_level(tint_smooth_brightness);
        }
        #endif

        return MISCHIEF_MANAGED;
    }
    // release hold: save new strobe settings
    else if (event == EV_click2_hold_release) {
        save_config();
        return MISCHIEF_MANAGED;
    }
    #ifdef USE_MOMENTARY_MODE
    // 8 clicks: go to momentary mode (momentary strobe)
    else if (event == EV_8clicks) {
        set_state(momentary_state, 0);
        set_level(0);
        return MISCHIEF_MANAGED;
    }
    #endif
    #if defined(USE_LIGHTNING_MODE) || defined(USE_CANDLE_MODE)
    // clock tick: bump the random seed
    else if (event == EV_tick) {
        // un-reverse after 1 second
        if (arg == AUTO_REVERSE_TIME) ramp_direction = 1;

        pseudo_rand_seed += arg;
        return MISCHIEF_MANAGED;
    }
    // 4C: turning down busy factor (less busy) of lightning mode,
    //  or turning down firework brightness by 12,
    //  or decrease tint alternating interval by 0.5 second
    else if (event == EV_4clicks) {
        if (0) {}  // placeholder
        #ifdef USE_LIGHTNING_MODE
        else if (st == lightning_storm_e) {
            lightning_busy_factor++;
            if (lightning_busy_factor > LIGHTNING_BUSY_FACTOR_MAX)
                lightning_busy_factor = LIGHTNING_BUSY_FACTOR_MAX;
            save_config();
            blink_once();
        }
        #endif
        #ifdef USE_FIREWORK_MODE
        else if (st == firework_mode_e) {
            firework_brightness -= 12;
            if (firework_brightness < MIN_FIREWORK_LEVEL)
                firework_brightness = MIN_FIREWORK_LEVEL;
            save_config();
            blink_once();
        }
        #endif
        #ifdef USE_TINT_RAMPING
        else if (st == tint_alternating_strobe_e) {
            if (tint_alt_interval > TINT_ALT_MIN_INTERVAL)
                tint_alt_interval--;
            save_config();
            blink_once();
        }
        else if (st == tint_smooth_ramp_e) {
            if (tint_smooth_pause > TINT_SMOOTH_MIN_PAUSE)
                tint_smooth_pause -= 2;
            save_config();
            blink_once();
        }
        #endif
        return MISCHIEF_MANAGED;
    }
    // 5C: turning up busy factor (busier) of lightning mode,
    //  or turning up firework brightness by 12,
    //  or increasing tint alternating interval by 0.5 second
    else if (event == EV_5clicks) {
        if (0) {}  // placeholder
        #ifdef USE_LIGHTNING_MODE
        else if (st == lightning_storm_e) {
            lightning_busy_factor--;
            if (lightning_busy_factor < LIGHTNING_BUSY_FACTOR_MIN)
                lightning_busy_factor = LIGHTNING_BUSY_FACTOR_MIN;
            save_config();
            blink_once();
        }
        #endif
        #ifdef USE_FIREWORK_MODE
        else if (st == firework_mode_e) {
            firework_brightness += 12;
            if (firework_brightness > MAX_LEVEL)
                firework_brightness = MAX_LEVEL;
            save_config();
            blink_once();
        }
        #endif
        #ifdef USE_TINT_RAMPING
        else if (st == tint_alternating_strobe_e) {
            if (tint_alt_interval < TINT_ALT_MAX_INTERVAL)
                tint_alt_interval++;
            save_config();
            blink_once();
        }
        else if (st == tint_smooth_ramp_e) {
            if (tint_smooth_pause < TINT_SMOOTH_MAX_PAUSE)
                tint_smooth_pause += 2;
            save_config();
            blink_once();
        }
        #endif
        return MISCHIEF_MANAGED;
    }
    // 6C: reset lightning busy factor to default,
    //  or reset firework brightness to default
    else if (event == EV_6clicks) {
        if (0) {}  // placeholder
        #ifdef USE_LIGHTNING_MODE
        else if (st == lightning_storm_e) {
            lightning_busy_factor = LIGHTNING_BUSY_FACTOR;
            save_config();
            blink_once();
        }
        #endif
        #ifdef USE_FIREWORK_MODE
        else if (st == firework_mode_e) {
            firework_brightness = RAMP_SMOOTH_CEIL;
        }
        #endif
        return MISCHIEF_MANAGED;
    }
    #endif
    return EVENT_NOT_HANDLED;
}

// runs repeatedly in FSM loop() whenever UI is in strobe_state or momentary strobe
inline void strobe_state_iter() {
    uint8_t st = strobe_type;  // can't use switch() on an enum

    switch(st) {
        #if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
        #ifdef USE_PARTY_STROBE_MODE
        case party_strobe_e:
        #endif
        #ifdef USE_TACTICAL_STROBE_MODE
        case tactical_strobe_e:
        #endif
            party_tactical_strobe_mode_iter(st);
            break;
        #endif

        #ifdef USE_LIGHTNING_MODE
        case lightning_storm_e:
            lightning_storm_iter();
            break;
        #endif

        #ifdef USE_BIKE_FLASHER_MODE
        case bike_flasher_e:
            bike_flasher_iter();
            break;
        #endif

        #ifdef USE_FIREWORK_MODE
        case firework_mode_e:
            firework_iter();
            break;
        #endif

        #ifdef USE_TINT_RAMPING
        case tint_alternating_strobe_e:
            tint_alt_iter();
            break;
        case tint_smooth_ramp_e:
            tint_smooth_ramp_iter();
        #endif
    }
}
#endif  // ifdef USE_STROBE_STATE

#if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
inline void party_tactical_strobe_mode_iter(uint8_t st) {
    // one iteration of main loop()
    uint8_t del = strobe_delays[st];
    // TODO: make tac strobe brightness configurable?
    set_level(STROBE_BRIGHTNESS);
    if (0) {}  // placeholde0
    #ifdef USE_PARTY_STROBE_MODE
    else if (st == party_strobe_e) {  // party strobe
        #ifdef PARTY_STROBE_ONTIME
        nice_delay_ms(PARTY_STROBE_ONTIME);
        #else
        if (del < 42) delay_zero();
        else nice_delay_ms(1);
        #endif
    }
    #endif
    #ifdef USE_TACTICAL_STROBE_MODE
    else {  //tactical strobe
        nice_delay_ms(del >> 1);
    }
    #endif
    set_level(STROBE_OFF_LEVEL);
    nice_delay_ms(del);  // no return check necessary on final delay
}
#endif

#ifdef USE_LIGHTNING_MODE
inline void lightning_storm_iter() {
    // one iteration of main loop()
    int16_t brightness;
    uint16_t rand_time;

    // turn the emitter on at a random level,
    // for a random amount of time between 1ms and 32ms
    //rand_time = 1 << (pseudo_rand() % 7);
    rand_time = pseudo_rand() & 63;
    brightness = 1 << (pseudo_rand() % 7);  // 1, 2, 4, 8, 16, 32, 64
    brightness += 1 << (pseudo_rand() % 5);  // 2 to 80 now
    brightness += pseudo_rand() % brightness;  // 2 to 159 now (w/ low bias)
    if (brightness > MAX_LEVEL) brightness = MAX_LEVEL;
    set_level(brightness);
    nice_delay_ms(rand_time);

    // decrease the brightness somewhat more gradually, like lightning
    uint8_t stepdown = brightness >> 3;
    if (stepdown < 1) stepdown = 1;
    while(brightness > 1) {
        nice_delay_ms(rand_time);
        brightness -= stepdown;
        if (brightness < 0) brightness = 0;
        set_level(brightness);
        /*
           if ((brightness < MAX_LEVEL/2) && (! (pseudo_rand() & 15))) {
           brightness <<= 1;
           set_level(brightness);
           }
           */
        if (! (pseudo_rand() & 3)) {
            nice_delay_ms(rand_time);
            set_level(brightness>>1);
        }
    }

    // turn the emitter off,
    // for a random amount of time between 1ms and 8192ms (default busy factor)
    // (with a low bias)
    rand_time = 1 << (pseudo_rand() % lightning_busy_factor);
    rand_time += pseudo_rand() % rand_time;
    set_level(0);
    nice_delay_ms(rand_time);  // no return check necessary on final delay
}
#endif

#ifdef USE_BIKE_FLASHER_MODE
inline void bike_flasher_iter() {
    // one iteration of main loop()
    uint8_t burst = bike_flasher_brightness << 1;
    if (burst > MAX_LEVEL) burst = MAX_LEVEL;
    for(uint8_t i=0; i<4; i++) {
        set_level(burst);
        nice_delay_ms(5);
        set_level(bike_flasher_brightness);
        nice_delay_ms(65);
    }
    nice_delay_ms(720);  // no return check necessary on final delay
    set_level(0);
}
#endif

#ifdef USE_FIREWORK_MODE
#define FIREWORK_DEFAULT_STAGE_COUNT 64
#define FIREWORK_DEFAULT_INTERVAL (2500/FIREWORK_DEFAULT_STAGE_COUNT)
uint8_t firework_stage = 0;
uint8_t firework_stage_count = FIREWORK_DEFAULT_STAGE_COUNT;
uint8_t step_interval = FIREWORK_DEFAULT_INTERVAL;
#ifdef USE_TINT_RAMPING
uint8_t tint_mode = 0; // 0: alternate, 1: channel 1 only, 2: channel 2 only, 3: random tint
uint8_t chosen_tint = 127; // for tint_mode = 3
#endif

inline static void update_firework_tint(uint8_t stage) {
    #ifdef USE_TINT_RAMPING
    if (tint_locked) return;
    switch (tint_mode) {
    // alternate both channels
    case 0:
        if (stage % 5 == 0)
            tint = (tint == 1) ? 254 : 1;
        break;
    // channel 1 only
    case 1:
        tint = 1;
        break;
    // channel 2 only
    case 2:
        tint = 254;
        break;
    // random tint
    case 3:
        tint = chosen_tint;
        break;
    }
    #endif
}
// code is copied and modified from factory-reset.c
inline void firework_iter() {
    if (firework_stage == firework_stage_count) {
        // explode, and reset stage
        firework_stage = 0;
        for (uint8_t brightness = firework_brightness; brightness > 0; brightness--) {
            update_firework_tint(firework_stage);
            set_level(brightness);
            nice_delay_ms(step_interval/4);
            set_level((uint16_t)brightness*7/8);
            nice_delay_ms(step_interval/(1+(pseudo_rand()%5)));
        }
        // off for 1 to 5 seconds
        set_level(0);
        nice_delay_ms(1000 + (pseudo_rand() % 5) * 1000);
        #ifdef USE_TINT_RAMPING
        // randomly set a tint mode for the next firework show
        tint_mode = pseudo_rand() % 4;
        if (tint_mode == 3) chosen_tint = pseudo_rand() % 254 + 1;
        update_firework_tint(firework_stage);
        #endif
        // set next stage count (16 to 64 in increment of 8)
        firework_stage_count = 16 + 8 * (pseudo_rand() % 7);
        return;
    }
    // wind up to explode
    set_level(firework_stage);
    nice_delay_ms(step_interval/3);
    set_level((uint16_t)firework_stage*2/3);
    nice_delay_ms(step_interval/3);
    firework_stage++;
    // we've reached our max brightness for firework mode, let's explode in the next iteration
    if (firework_stage > firework_brightness)
        firework_stage = firework_stage_count;
}
#endif

#ifdef USE_TINT_RAMPING
inline void tint_alt_iter() {
    if (tint_locked) return;

    tint = (tint == 1) ? 254 : 1;
    set_level(tint_alt_brightness);
    nice_delay_ms(500 * tint_alt_interval);
}

inline void tint_smooth_ramp_iter() {
    if (tint_locked) return;

    static uint8_t tint_step = 1;
    if (tint == 254) tint_step = -1;
    else if (tint == 1) tint_step = 1;

    tint += tint_step;
    set_level(tint_smooth_brightness);
    nice_delay_ms(tint_smooth_pause);
}
#endif

#ifdef USE_CANDLE_MODE
#include "candle-mode.c"
#endif


#ifdef USE_BORING_STROBE_STATE
#include "ff-strobe-modes.c"
#endif


#endif

