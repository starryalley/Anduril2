/*
 * tint-ramping.c: Tint ramping functions for Anduril.
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

#ifndef TINT_RAMPING_C
#define TINT_RAMPING_C

#include "tint-ramping.h"

uint8_t tint_ramping_state(Event event, uint16_t arg) {
    static int8_t tint_ramp_direction = 1;
    // don't activate auto-tint modes unless the user hits the edge
    // and keeps pressing for a while
    static uint8_t past_edge_counter = 0;
    // bugfix: click-click-hold from off to strobes would invoke tint ramping
    // in addition to changing state...  so ignore any tint-ramp events which
    // don't look like they were meant to be here
    static uint8_t active = 0;

    // click, click, hold: change the tint
    if (event == EV_click3_hold) {
        // reset at beginning of movement
        if (!arg) {
            active = 1;  // first frame means this is for us
            past_edge_counter = 0;  // doesn't start until user hits the edge
        }
        // ignore event if we weren't the ones who handled the first frame
        if (!active) return EVENT_HANDLED;

        const uint8_t step_size = (tint_style < 2) ? 1 : 253 / (tint_style-1);

        if ((tint_ramp_direction > 0 && tint < 254) ||
                (tint_ramp_direction < 0 && tint > 1)) {
            // ramp slower in discrete mode
            if (tint_style && (arg % HOLD_TIMEOUT != 0)) return EVENT_HANDLED;

            tint = nearest_tint_value(tint + step_size*tint_ramp_direction);
        } else {
            // if tint change stalled, let user know we hit the edge
            if (past_edge_counter == 0) blip();
            if (past_edge_counter < 255) past_edge_counter++;
        }

        // if the user kept pressing long enough, go the final step
        if (!tint_style && past_edge_counter == 3*HOLD_TIMEOUT) {
            tint ^= 1;  // 1 -> 0, 254 -> 255
            blip();
        }

        set_level(actual_level);
        return EVENT_HANDLED;
    }

    // click, click, hold, release: reverse direction for next ramp
    else if (event == EV_click3_hold_release) {
        active = 0;  // ignore next hold if it wasn't meant for us
        // reverse
        tint_ramp_direction = -tint_ramp_direction;
        if (tint <= 1) tint_ramp_direction = 1;
        else if (tint >= 254) tint_ramp_direction = -1;
        // remember tint after battery change
        save_config();
        // bug?: for some reason, brightness can seemingly change
        // from 1/150 to 2/150 without this next line... not sure why
        set_level(actual_level);
        return EVENT_HANDLED;
    }
    // 8H to go to middle tint
    else if (event == EV_click8_hold) {
      tint = 127;
      set_level(actual_level);
      return EVENT_HANDLED;
    }

    return EVENT_NOT_HANDLED;
}

uint8_t nearest_tint_value(const int16_t target) {
    // constexpr for more readable code, will be removed by the compiler
    const uint8_t tint_min = 1;
    const uint8_t tint_max = 254;
    const uint8_t tint_range = tint_max - tint_min;

    // only equal mix of both channels
    if (tint_style == 1) return (tint_min + tint_max) >> 1;

    if (target < tint_min) return tint_min;
    if (target > tint_max) return tint_max;
    if (!tint_style) return target;  // nothing more to do for smooth ramping

    const uint8_t step_size = tint_range / (tint_style-1);

    uint8_t tint_result = tint_min;
    for (uint8_t i=0; i<tint_style; i++) {
        tint_result = tint_min + i * (uint16_t)tint_range / (tint_style-1);
        int16_t diff = target - tint_result;
        if (diff <= (step_size>>1))
            return tint_result;
    }
    return tint_result;
}


#endif

