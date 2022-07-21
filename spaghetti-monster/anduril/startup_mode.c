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
    } else if (ramp_start_mode == 2) {
        // TODO: set additional parameter for fluorescent startup flickering
        set_level_and_therm_target(lvl);
    } else {
        // 0 or else: default behaviour: instant on
        set_level_and_therm_target(lvl);
    }
}

void ramp_up_tick() {
    if (ramp_start_mode == 1) {
        if (ramp_up_target > 0 && ramp_up_target > actual_level) {
            if (actual_level+ramp_up_increment >= ramp_up_target) {
                set_level(ramp_up_target);
                // we're done now
                ramp_up_target = 0;
            } else {
                set_level(actual_level + ramp_up_increment);
            }
        }
    } else if (ramp_start_mode == 2) {
        // TODO: fluorescent startup flickering
    }
}

#endif