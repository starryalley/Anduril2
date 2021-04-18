// Noctigon KR4 config options for Anduril
// (and Emisar D4v2.5, which uses KR4 driver plus a button LED)
#define MODEL_NUMBER "0211"
#include "hwdef-Noctigon_KR4.h"
// ATTINY: 1634

// this light has three aux LED channels: R, G, B
#define USE_AUX_RGB_LEDS
// the aux LEDs are front-facing, so turn them off while main LEDs are on
//#define USE_AUX_RGB_LEDS_WHILE_ON
// it also has an independent LED in the button (D4v2.5 titanium/brass only)
#define USE_BUTTON_LED
// TODO: the whole "indicator LED" thing needs to be refactored into
//       "aux LED(s)" and "button LED(s)" since they work a bit differently
// enabling this option breaks the button LED on D4v2.5
#ifdef USE_INDICATOR_LED_WHILE_RAMPING
#undef USE_INDICATOR_LED_WHILE_RAMPING
#endif

// enable blinking aux LEDs
#define TICK_DURING_STANDBY
#define STANDBY_TICK_SPEED 3  // every 0.128 s


// brightness w/ SST-20 4000K LEDs:
// 0/1023: 0.35 lm
// 1/1023: 2.56 lm
// max regulated: 1740 lm
// FET: ~3700 lm
// maxreg at 130: level_calc.py cube 2 150 7135 0 2.5 1740 FET 1 10 2565
// maxreg at 120: level_calc.py cube 2 150 7135 0 2.5 1740 FET 1 10 3190
#define RAMP_LENGTH 150
#define PWM1_LEVELS 0,0,1,1,2,2,3,3,4,4,5,6,7,8,9,10,11,13,14,15,17,19,20,22,24,26,28,30,33,35,38,40,43,46,49,52,55,59,62,66,70,74,78,82,86,91,96,100,105,111,116,121,127,133,139,145,151,158,165,172,179,186,193,201,209,217,225,234,243,251,261,270,280,289,299,310,320,331,342,353,364,376,388,400,412,425,438,451,464,478,492,506,521,536,551,566,582,597,614,630,647,664,681,699,717,735,754,772,792,811,831,851,871,892,913,935,956,978,1001,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,1023,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,51,79,109,138,168,198,229,260,292,324,357,390,423,457,492,527,562,598,634,671,708,746,784,822,861,901,941,982,1023
#define DEFAULT_LEVEL 46
#define MAX_1x7135 120
#define HALFSPEED_LEVEL 10
#define QUARTERSPEED_LEVEL 2

#define RAMP_SMOOTH_FLOOR 3  // level 1 is unreliable
#define RAMP_SMOOTH_CEIL  120
// 10, 28, [46], 65, 83, 101, [120]
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit ~30% power / ~1400 lm (can sustain 900 lm)
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL 110
#define SIMPLE_UI_STEPS 5

// stop panicking at ~25% power or ~1000 lm
#define THERM_FASTER_LEVEL 100
#define MIN_THERM_STEPDOWN DEFAULT_LEVEL
#define THERM_NEXT_WARNING_THRESHOLD 16  // accumulate less error before adjusting
#define THERM_RESPONSE_MAGNITUDE 128  // bigger adjustments

// slow down party strobe; this driver can't pulse for 1ms or less
// (only needed on no-FET build)
//#define PARTY_STROBE_ONTIME 2

#define THERM_CAL_OFFSET 5

// can't reset the normal way because power is connected before the button
#define USE_SOFT_FACTORY_RESET
