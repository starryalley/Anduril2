/*
 * fsm-main.c: main() function for SpaghettiMonster.
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

#ifndef FSM_MAIN_C
#define FSM_MAIN_C

#include "fsm-main.h"

#if PWM_CHANNELS == 4
#ifdef AVRXMEGA3  // ATTINY816, 817, etc
#error 4-channel PWM not currently set up for the AVR 1-Series
#endif
// 4th PWM channel requires manually turning the pin on/off via interrupt :(
ISR(TIMER1_OVF_vect) {
    //bitClear(PORTB, 3);
    PORTB &= 0b11110111;
    //PORTB |= 0b00001000;
}
ISR(TIMER1_COMPA_vect) {
    //if (!bitRead(TIFR,TOV1)) bitSet(PORTB, 3);
    if (! (TIFR & (1<<TOV1))) PORTB |= 0b00001000;
    //if (! (TIFR & (1<<TOV1))) PORTB &= 0b11110111;
}
#endif

#if (ATTINY == 25) || (ATTINY == 45) || (ATTINY == 85)
static inline void hw_setup() {
    // configure PWM channels
    #if PWM_CHANNELS >= 1
    DDRB |= (1 << PWM1_PIN);
    TCCR0B = 0x01; // pre-scaler for timer (1 => 1, 2 => 8, 3 => 64...)
    TCCR0A = PHASE;
    #endif
    #if PWM_CHANNELS >= 2
    DDRB |= (1 << PWM2_PIN);
    #endif
    #if PWM_CHANNELS >= 3
    // Second PWM counter is ... weird
    DDRB |= (1 << PWM3_PIN);
    TCCR1 = _BV (CS10);
    GTCCR = _BV (COM1B1) | _BV (PWM1B);
    OCR1C = 255;  // Set ceiling value to maximum
    #endif
    #if PWM_CHANNELS >= 4
    // 4th PWM channel is ... not actually supported in hardware  :(
    DDRB |= (1 << PWM4_PIN);
    //OCR1C = 255;  // Set ceiling value to maximum
    TCCR1 = 1<<CTC1 | 1<<PWM1A | 3<<COM1A0 | 2<<CS10;
    GTCCR = (2<<COM1B0) | (1<<PWM1B);
    // set up an interrupt to control PWM4 pin
    TIMSK |= (1<<OCIE1A) | (1<<TOIE1);
    #endif

    // configure e-switch
    PORTB = (1 << SWITCH_PIN);  // e-switch is the only input
    PCMSK = (1 << SWITCH_PIN);  // pin change interrupt uses this pin
}
#elif (ATTINY == 1634) || defined(AVRXMEGA3)  // ATTINY816, 817, etc
static inline void hw_setup() {
    // this gets tricky with so many pins...
    // ... so punt it to the hwdef file
    hwdef_setup();
}
#else
    #error Unrecognized MCU type
#endif


//#ifdef USE_REBOOT
static inline void prevent_reboot_loop() {
    // prevent WDT from rebooting MCU again
    #ifdef AVRXMEGA3  // ATTINY816, 817, etc
    RSTCTRL.RSTFR &= ~(RSTCTRL_WDRF_bm);  // reset status flag
    #else
    MCUSR &= ~(1<<WDRF);  // reset status flag
    #endif
    wdt_disable();
}
//#endif


int main() {
    // Don't allow interrupts while booting
    cli();

    //#ifdef USE_REBOOT
    // prevents cycling after a crash,
    // whether intentional (like factory reset) or not (bugs)
    prevent_reboot_loop();
    //#endif

    hw_setup();

    #if 0
    #ifdef HALFSPEED
    // run at half speed
    // FIXME: not portable (also not needed)
    CLKPR = 1<<CLKPCE;
    CLKPR = 1;
    #endif
    #endif

    #ifdef USE_DEBUG_BLINK
    //debug_blink(1);
    #endif

    // all booted -- turn interrupts back on
    PCINT_on();
    WDT_on();
    ADC_on();
    sei();

    // in case any spurious button presses were detected at boot
    #ifdef USE_DELAY_MS
    delay_ms(1);
    #else
    delay_4ms(1);
    #endif

    // fallback for handling a few things
    #ifndef DONT_USE_DEFAULT_STATE
    push_state(default_state, 0);
    nice_delay_interrupt = 0;
    #endif

    // call recipe's setup
    setup();

    // main loop
    while (1) {
        // if event queue not empty, empty it
        process_emissions();

        // if loop() tried to change state, process that now
        StatePtr df = deferred_state;
        if (df) {
            set_state(df, deferred_state_arg);
            deferred_state = NULL;
            //deferred_state_arg = 0;  // unnecessary
        }

        // enter standby mode if requested
        // (works better if deferred like this)
        if (go_to_standby) {
            #ifdef USE_RAMPING
            set_level(0);
            #else
            #if PWM_CHANNELS >= 1
            PWM1_LVL = 0;
            #endif
            #if PWM_CHANNELS >= 2
            PWM2_LVL = 0;
            #endif
            #if PWM_CHANNELS >= 3
            PWM3_LVL = 0;
            #endif
            #if PWM_CHANNELS >= 4
            PWM4_LVL = 255;  // inverted  :(
            #endif
            #endif
            standby_mode();
        }

        // catch up on interrupts
        handle_deferred_interrupts();

        // turn delays back on, if they were off
        nice_delay_interrupt = 0;

        // give the recipe some time slices
        loop();

    }
}


void handle_deferred_interrupts() {
    /*
    if (irq_pcint) {  // button pressed or released
        // nothing to do here
        // (PCINT only matters during standby)
    }
    */
    if (irq_adc) {  // ADC done measuring
        adc_deferred();
        // irq_adc = 0;  // takes care of itself
    }
    if (irq_wdt) {  // the clock ticked
        WDT_inner();
        // irq_wdt = 0;  // takes care of itself
    }
}

#endif
