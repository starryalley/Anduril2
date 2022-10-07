/*
 * startup_mode.h: ramp startup mode
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

#ifndef STARTUP_MODE_H
#define STARTUP_MODE_H

#ifndef DEFAULT_RAMP_START_MODE
#define DEFAULT_RAMP_START_MODE 1
#endif

// 0: instant on (default)
// 1: ramp up to memorized level within 0.5sec
// 2: old fluorescent light flicker
// 3: lighter flicker and slowly ramp up
uint8_t ramp_start_mode = DEFAULT_RAMP_START_MODE;
uint8_t ramp_up_target = 0;
uint8_t ramp_up_increment = 1;
uint8_t flicker_random = 1;
uint8_t flicker_index = 0;
void ramp_up_level(uint8_t lvl);
void ramp_up_tick();

#endif
