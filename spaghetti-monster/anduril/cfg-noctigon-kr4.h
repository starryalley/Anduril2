// Noctigon KR4 config options for Anduril
// (and Emisar D4v2.5, which uses KR4 driver plus a button LED)
#define MODEL_NUMBER "0211"
#include "hwdef-Noctigon_KR4.h"
#include "hank-cfg.h"
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

// use 12-bit PWM at 1kHz
#undef PWM_BITS
#define PWM_BITS 12
#undef PWM_TOP
#define PWM_TOP 0xFFF //4095

// brightness w/ SST-20 4000K LEDs:
// 0/1023: 0.35 lm
// 1/1023: 2.56 lm
// max regulated: 1740 lm
// FET: ~3700 lm
// length 150:
// maxreg at 130: level_calc.py cube 2 150 7135 0 2.5 1023 1740 FET 1 10 1023 2565
// maxreg at 120: level_calc.py cube 2 150 7135 0 2.5 1023 1740 FET 1 10 1023 3190
// length 200:
// maxreg at 120: level_calc.py cube 2 200 7135 0 2.5 4095 1740 FET 1 10 4095 3190
#define RAMP_LENGTH 200
#define PWM1_LEVELS 0,1,2,3,4,6,7,8,10,12,14,16,18,20,23,25,28,31,34,37,41,44,48,52,56,60,65,69,74,79,84,90,96,102,108,114,121,128,135,142,150,158,166,174,183,192,201,211,221,231,241,252,263,274,286,298,310,323,336,349,362,376,391,405,420,436,451,468,484,501,518,536,554,572,591,610,630,650,670,691,712,734,756,779,802,825,849,874,898,924,950,976,1003,1030,1057,1086,1114,1143,1173,1203,1234,1265,1297,1329,1362,1395,1429,1463,1498,1534,1570,1606,1644,1681,1720,1758,1798,1838,1878,1920,1961,2004,2047,2090,2135,2180,2225,2271,2318,2365,2413,2462,2511,2561,2612,2663,2715,2767,2821,2874,2929,2984,3040,3097,3154,3213,3271,3331,3391,3452,3514,3576,3639,3703,3768,3833,3899,3966,4034,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,0
#define PWM2_LEVELS 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65,149,234,320,408,496,585,675,766,858,951,1045,1140,1236,1332,1430,1529,1629,1730,1832,1935,2039,2144,2251,2358,2466,2575,2686,2797,2910,3023,3138,3254,3370,3488,3608,3728,3849,3971,4095
#define DEFAULT_LEVEL 60
#define MAX_1x7135 160
#define HALFSPEED_LEVEL 10
#define QUARTERSPEED_LEVEL 2

#define RAMP_SMOOTH_FLOOR 1  // level 1 is unreliable
#define RAMP_SMOOTH_CEIL  160
// 10, 28, [46], 65, 83, 101, [120]
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit ~30% power / ~1400 lm (can sustain 900 lm)
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL 160
#define SIMPLE_UI_STEPS 5

// stop panicking at ~25% power or ~1000 lm
#define THERM_FASTER_LEVEL 130
#define MIN_THERM_STEPDOWN DEFAULT_LEVEL
#define THERM_NEXT_WARNING_THRESHOLD 16  // accumulate less error before adjusting
#define THERM_RESPONSE_MAGNITUDE 128  // bigger adjustments

// slow down party strobe; this driver can't pulse for 1ms or less
// (only needed on no-FET build)
//#define PARTY_STROBE_ONTIME 2

#define THERM_CAL_OFFSET 5

// can't reset the normal way because power is connected before the button
#define USE_SOFT_FACTORY_RESET

#define USE_UNLOCK_TO_OFF

// disable unused strobe modes
#undef USE_PARTY_STROBE_MODE
#undef USE_TACTICAL_STROBE_MODE

// additional strobe modes
#define USE_FIREWORK_MODE
