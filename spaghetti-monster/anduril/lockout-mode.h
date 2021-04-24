/*
 * lockout-mode.h: Lockout mode for Anduril.
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

#ifndef LOCKOUT_MODE_H
#define LOCKOUT_MODE_H

// soft lockout
uint8_t lockout_state(Event event, uint16_t arg);

#ifdef USE_AUTOLOCK
uint8_t autolock_time = 0;
uint8_t autolock_config_state(Event event, uint16_t arg);
#endif
#ifdef USE_BUTTON_LED
extern uint8_t blink_button_comfort_temperature;
#endif

#endif
