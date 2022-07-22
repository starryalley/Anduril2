/*
 * startup_mode.c: ramp startup mode
 *
 * Copyright (C) 2022 Mark Kuo
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

#ifndef STARTUP_MODE_C
#define STARTUP_MODE_C

#include "startup_mode.h"

void ramp_up_level(uint8_t lvl) {
    if (ramp_start_mode == 1) {
        ramp_up_target = lvl;
        ramp_up_increment = ramp_up_target / (TICKS_PER_SECOND/4) + 1;
    } else if (ramp_start_mode == 2 || ramp_start_mode == 3) {
        ramp_up_target = lvl;
        flicker_random = pseudo_rand()%20 + 1;
        ramp_up_increment = 0;
        flicker_index = 0;
    } else {
        // 0 or else: default behaviour: instant on
        set_level_and_therm_target(lvl);
    }
}

void ramp_up_tick() {
    if (ramp_up_target == 0) return;

    // even index: light off, odd index: light on
    // unit: tick (1/62s) or -1 means random number (1~30 ticks) generated at boot
    static const int8_t fluorescent_pattern[] = {-1,2, 5,3, -1,1, -1,2, -1,5, 14,-1, /*6*/-1};
    // more broken fluorescent
    //static const int8_t fluorescent_pattern[] = {1,4, -1,2, 5,3, -1,5, 7,27, -1,5, 5,10, -1,20, 34,-1, 2};
    // lighter
    static const int8_t lighter_pattern[] = {5,2, 12,2, -1,1};

    switch(ramp_start_mode) {
    case 1:
        if (ramp_up_target > actual_level) {
            if (actual_level+ramp_up_increment >= ramp_up_target) {
                set_level(ramp_up_target);
                // we're done now
                ramp_up_target = 0;
            } else {
                set_level(actual_level + ramp_up_increment);
            }
        }
        break;
    case 2:
        // fluorescent startup flickering
        ramp_up_increment++;
        if ((fluorescent_pattern[flicker_index] == -1 && ramp_up_increment == flicker_random) ||
            (fluorescent_pattern[flicker_index] == ramp_up_increment)) {
            flicker_index++;
            ramp_up_increment = 0;
            set_level(flicker_index & 1 ? ramp_up_target>>1: 0);
        }
        if (flicker_index == sizeof(fluorescent_pattern)) {
            // we're done now
            set_level(ramp_up_target);
            ramp_up_target = 0;
            flicker_index = 0;
        }
        break;
    case 3:
        // lighter flickering
        ramp_up_increment++;
        // step 1: flicker a bit
        if (flicker_index < sizeof(lighter_pattern) && (
            (lighter_pattern[flicker_index] == -1 && ramp_up_increment == flicker_random) ||
            (lighter_pattern[flicker_index] == ramp_up_increment))) {
            flicker_index++;
            ramp_up_increment = 0;
            set_level(flicker_index & 1 ? ramp_up_target>>1: 0);
        }
        // step 2: ramp up slowly to target
        if (flicker_index == sizeof(lighter_pattern)) {
            // ramp up slowly now
            set_level(actual_level + 2);
            if (actual_level >= ramp_up_target) {
                // all done
                set_level(ramp_up_target);
                ramp_up_target = 0;
                flicker_index = 0;
            }
        }
        break;
    }
}

#endif