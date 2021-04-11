/*
 * off-mode.h: "Off" mode for Anduril.
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

#ifndef OFF_MODE_H
#define OFF_MODE_H

// when the light is "off" or in standby
uint8_t off_state(Event event, uint16_t arg);

extern uint8_t blink_brightness;
#ifdef USE_BUTTON_LED
extern uint8_t blink_button_comfort_temperature;
#endif
#endif
