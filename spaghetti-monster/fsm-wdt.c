/*
 * fsm-wdt.c: WDT (Watch Dog Timer) functions for SpaghettiMonster.
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

#ifndef FSM_WDT_C
#define FSM_WDT_C

#include <avr/interrupt.h>
#include <avr/wdt.h>

// *** Note for the AVRXMEGA3 (1-Series, eg 816 and 817), the WDT 
// is not used for time-based interrupts.  A new peripheral, the 
// Periodic Interrupt Timer ("PIT") is used for this purpose.

void WDT_on()
{
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85)
        // interrupt every 16ms
        //cli();                          // Disable interrupts
        wdt_reset();                    // Reset the WDT
        WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
        WDTCR = (1<<WDIE);              // Enable interrupt every 16ms
        //sei();                          // Enable interrupts
    #elif (ATTINY == 1634)
        wdt_reset();                    // Reset the WDT
        WDTCSR = (1<<WDIE);             // Enable interrupt every 16ms
    #elif defined(AVRXMEGA3)  // ATTINY816, 817, etc
        RTC.PITINTCTRL = RTC_PI_bm;   // enable the Periodic Interrupt
        while (RTC.PITSTATUS > 0) {}  // make sure the register is ready to be updated
        RTC.PITCTRLA = RTC_PERIOD_CYC512_gc | RTC_PITEN_bm; // Period = 16ms, enable the PI Timer
    #else
        #error Unrecognized MCU type
    #endif
}

#ifdef TICK_DURING_STANDBY
inline void WDT_slow()
{
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85)
        // interrupt slower
        //cli();                          // Disable interrupts
        wdt_reset();                    // Reset the WDT
        WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
        WDTCR = (1<<WDIE) | STANDBY_TICK_SPEED; // Enable interrupt every so often
        //sei();                          // Enable interrupts
    #elif (ATTINY == 1634)
        wdt_reset();                    // Reset the WDT
        WDTCSR = (1<<WDIE) | STANDBY_TICK_SPEED;
    #elif defined(AVRXMEGA3)  // ATTINY816, 817, etc
        RTC.PITINTCTRL = RTC_PI_bm;   // enable the Periodic Interrupt
        while (RTC.PITSTATUS > 0) {}  // make sure the register is ready to be updated
        RTC.PITCTRLA = (1<<6) | (STANDBY_TICK_SPEED<<3) | RTC_PITEN_bm; // Set period, enable the PI Timer
    #else
        #error Unrecognized MCU type
    #endif
}
#endif

inline void WDT_off()
{
    #if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85)
        //cli();                          // Disable interrupts
        wdt_reset();                    // Reset the WDT
        MCUSR &= ~(1<<WDRF);            // Clear Watchdog reset flag
        WDTCR |= (1<<WDCE) | (1<<WDE);  // Start timed sequence
        WDTCR = 0x00;                   // Disable WDT
        //sei();                          // Enable interrupts
    #elif (ATTINY == 1634)
        cli();                // needed because CCP, below
        wdt_reset();          // Reset the WDT
        MCUSR &= ~(1<<WDRF);  // clear watchdog reset flag
        CCP = 0xD8;           // enable config changes
        WDTCSR = 0;           // disable and clear all WDT settings
        sei();
    #elif defined(AVRXMEGA3)  // ATTINY816, 817, etc
        while (RTC.PITSTATUS > 0) {}  // make sure the register is ready to be updated
        RTC.PITCTRLA = 0; // Disable the PI Timer
    #else
        #error Unrecognized MCU type
    #endif
}

// clock tick -- this runs every 16ms (62.5 fps)
#ifdef AVRXMEGA3  // ATTINY816, 817, etc
ISR(RTC_PIT_vect) {
    RTC.PITINTFLAGS = RTC_PI_bm; // clear the PIT interrupt flag 
#else
ISR(WDT_vect) {
#endif
    irq_wdt = 1;  // WDT event happened
}

void WDT_inner() {
    irq_wdt = 0;  // WDT event handled; reset flag

    static uint8_t adc_trigger = 0;

    // cache this here to reduce ROM size, because it's volatile
    uint16_t ticks_since_last = ticks_since_last_event;
    // increment, but loop from max back to half
    ticks_since_last = (ticks_since_last + 1) \
                     | (ticks_since_last & 0x8000);
    // copy back to the original
    ticks_since_last_event = ticks_since_last;

    // detect and emit button change events (even during standby)
    uint8_t was_pressed = button_last_state;
    uint8_t pressed = button_is_pressed();
    if (was_pressed != pressed) {
        go_to_standby = 0;
        PCINT_inner(pressed);
    }
    // cache again, in case the value changed
    ticks_since_last = ticks_since_last_event;

    #ifdef TICK_DURING_STANDBY
    // handle standby mode specially
    if (go_to_standby) {
        // emit a sleep tick, and process it
        emit(EV_sleep_tick, ticks_since_last);
        process_emissions();

        #ifndef USE_SLEEP_LVP
        return;  // no sleep LVP needed if nothing drains power while off
        #else
        // stop here, usually...  but proceed often enough for sleep LVP to work
        if (0 != (ticks_since_last & 0x3f)) return;

        adc_trigger = 0;  // make sure a measurement will happen
        ADC_on();  // enable ADC voltage/temperature measurement functions temporarily
        #endif
    }
    else {  // button handling should only happen while awake
    #endif

    // if time since last event exceeds timeout,
    // append timeout to current event sequence, then
    // send event to current state callback

    // callback on each timer tick
    if ((current_event & B_FLAGS) == (B_CLICK | B_HOLD | B_PRESS)) {
        emit(EV_tick, 0);  // override tick counter while holding button
    }
    else {
        emit(EV_tick, ticks_since_last);
    }

    // user held button long enough to count as a long click?
    if (current_event & B_PRESS) {
        // during a "hold", send a hold event each tick, with a timer
        if (current_event & B_HOLD) {
            emit_current_event(ticks_since_last);
        }
        // has button been down long enough to become a "hold"?
        // (first frame of a "hold" event)
        else {
            if (ticks_since_last >= HOLD_TIMEOUT) {
                ticks_since_last_event = 0;
                current_event |= B_HOLD;
                emit_current_event(0);
            }
        }
    }

    // event in progress, but button not currently down
    else if (current_event) {
        // "hold" event just ended
        // no timeout required when releasing a long-press
        if (current_event & B_HOLD) {
            //emit_current_event(ticks_since_last);  // should have been emitted by PCINT_inner()
            empty_event_sequence();
        }
        // end and clear event after release timeout
        else if (ticks_since_last >= RELEASE_TIMEOUT) {
            current_event |= B_TIMEOUT;
            emit_current_event(0);
            empty_event_sequence();
        }
    }

    #ifdef TICK_DURING_STANDBY
    }
    #endif

    #if defined(USE_LVP) || defined(USE_THERMAL_REGULATION)
    // enable the deferred ADC handler once in a while
    if (! adc_trigger) {
        ADC_start_measurement();
        adc_deferred_enable = 1;
    }
    // timing for the ADC handler is every 32 ticks (~2Hz)
    adc_trigger = (adc_trigger + 1) & 31;
    #endif
}

#endif
