/*
 * strobe-modes.h: Strobe modes for Anduril.
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

#ifndef STROBE_MODES_H
#define STROBE_MODES_H

// internal numbering for strobe modes
// order: candle->lightning storm->firework->bike flasher->party->tactical
#ifdef USE_STROBE_STATE
typedef enum {
    #ifdef USE_PARTY_STROBE_MODE
    party_strobe_e,
    #endif
    #ifdef USE_TACTICAL_STROBE_MODE
    tactical_strobe_e,
    #endif
    #ifdef USE_CANDLE_MODE
    candle_mode_e,
    #endif
    #ifdef USE_LIGHTNING_MODE
    lightning_storm_e,
    #endif
    #ifdef USE_FIREWORK_MODE
    firework_mode_e,
    #endif
    #ifdef USE_BIKE_FLASHER_MODE
    bike_flasher_e,
    #endif
    strobe_mode_END
} strobe_mode_te;

const int NUM_STROBES = strobe_mode_END;

// which strobe mode is active?
#ifdef USE_CANDLE_MODE
strobe_mode_te strobe_type = candle_mode_e;
#else
strobe_mode_te strobe_type = 0;
#endif
#endif


// full FET strobe can be a bit much...  use max regulated level instead,
// if there's a bright enough regulated level
#ifndef STROBE_BRIGHTNESS
#ifdef MAX_Nx7135
#define STROBE_BRIGHTNESS MAX_Nx7135
#else
#define STROBE_BRIGHTNESS MAX_LEVEL
#endif
#endif

// party and tactical strobes
#ifdef USE_STROBE_STATE
uint8_t strobe_state(Event event, uint16_t arg);
inline void strobe_state_iter();
#endif

#if defined(USE_PARTY_STROBE_MODE) || defined(USE_TACTICAL_STROBE_MODE)
// party / tactical strobe timing
uint8_t strobe_delays[] = { 41, 67 };  // party strobe 24 Hz, tactical strobe 10 Hz
inline void party_tactical_strobe_mode_iter(uint8_t st);
#endif

#ifdef USE_LIGHTNING_MODE
#ifndef LIGHTNING_BUSY_FACTOR
#define LIGHTNING_BUSY_FACTOR 14 // max 16384ms (max ~16 sec interval)
#endif
uint8_t lightning_busy_factor = LIGHTNING_BUSY_FACTOR;
#define LIGHTNING_BUSY_FACTOR_MAX 16 // max 65536ms (max ~65 sec interval)
#define LIGHTNING_BUSY_FACTOR_MIN 12 // max 4096ms (max ~4 sec interval)
inline void lightning_storm_iter();
#endif

// bike mode config options
#ifdef USE_BIKE_FLASHER_MODE
#define MAX_BIKING_LEVEL 120  // should be 127 or less
uint8_t bike_flasher_brightness = MAX_1x7135;
inline void bike_flasher_iter();
#endif

#ifdef USE_FIREWORK_MODE
#define MIN_FIREWORK_LEVEL DEFAULT_LEVEL // max is always MAX_LEVEL
uint8_t firework_brightness = MAX_1x7135;
inline void firework_iter();
#endif

#ifdef USE_CANDLE_MODE
#include "candle-mode.h"
#endif


#if defined(USE_POLICE_STROBE_MODE) || defined(USE_SOS_MODE_IN_FF_GROUP)
#define USE_BORING_STROBE_STATE
#include "ff-strobe-modes.h"
#endif

#endif
