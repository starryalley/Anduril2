// Noctigon KR4 (fetless) config options for Anduril
// (and Noctigon KR1)
// (and Emisar D4v2 E21A, a.k.a. "D4v2.5")
#include "cfg-noctigon-kr4.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0212"
// ATTINY: 1634

// use 12-bit PWM at 1kHz
#undef PWM_BITS
#define PWM_BITS 12
#undef PWM_TOP
#define PWM_TOP 0xFFF //4095

// brightness w/ SST-20 4000K LEDs:
// 0/1023: 0.35 lm
// 1/1023: 2.56 lm
// max regulated: 1740 lm
// level_calc.py 3.0 1 150 7135 0 5 1740
#undef PWM_CHANNELS
#define PWM_CHANNELS 1
#undef RAMP_LENGTH
#define RAMP_LENGTH 200
#undef PWM1_LEVELS
//#define PWM1_LEVELS 0,0,1,1,2,2,3,3,4,4,5,5,6,7,8,9,10,11,12,13,15,16,17,18,20,21,23,24,26,27,29,31,33,35,37,39,41,43,45,48,50,53,55,58,61,63,66,69,72,75,79,82,85,89,92,96,100,104,108,112,116,120,125,129,134,138,143,148,153,158,163,169,174,180,185,191,197,203,209,215,222,228,235,242,248,255,263,270,277,285,292,300,308,316,324,333,341,350,359,368,377,386,395,405,414,424,434,444,454,465,475,486,497,508,519,531,542,554,566,578,590,603,615,628,641,654,667,680,694,708,722,736,750,765,779,794,809,825,840,856,872,888,904,920,937,954,971,988,1005,1023
// level_calc.py 3.0 1 200 7135 0 0.3 4095 1200
#define PWM1_LEVELS 0,0,1,1,1,2,2,3,3,4,4,5,6,7,8,9,10,11,12,13,15,16,18,20,21,23,25,27,29,32,34,36,39,42,44,47,50,54,57,60,64,68,72,76,80,84,88,93,98,103,108,113,118,124,130,136,142,148,154,161,168,175,182,190,197,205,213,221,229,238,247,256,265,275,284,294,304,315,325,336,347,358,370,382,394,406,419,431,445,458,471,485,499,514,528,543,559,574,590,606,622,639,656,673,690,708,726,745,764,783,802,822,842,862,883,904,925,947,969,991,1014,1037,1060,1084,1108,1132,1157,1182,1207,1233,1259,1286,1313,1340,1368,1396,1424,1453,1482,1512,1542,1572,1603,1634,1666,1698,1730,1763,1796,1830,1864,1898,1933,1968,2004,2040,2077,2114,2151,2189,2227,2266,2306,2345,2385,2426,2467,2508,2550,2593,2636,2679,2723,2767,2812,2857,2903,2949,2996,3043,3091,3139,3188,3237,3287,3337,3388,3439,3491,3543,3596,3649,3703,3758,3812,3868,3924,3980,4037,4095
#undef PWM2_LEVELS
#undef DEFAULT_LEVEL
#define DEFAULT_LEVEL 60
#undef MAX_1x7135
#define MAX_1x7135 200

#undef RAMP_SMOOTH_FLOOR
#undef RAMP_SMOOTH_CEIL
#undef RAMP_DISCRETE_FLOOR
#undef RAMP_DISCRETE_CEIL
#undef RAMP_DISCRETE_STEPS

#define RAMP_SMOOTH_FLOOR 1
#define RAMP_SMOOTH_CEIL  180
// 10, 30, [50], 70, 90, 110, 130  (plus [150] on turbo)
#define RAMP_DISCRETE_FLOOR 10
#define RAMP_DISCRETE_CEIL  RAMP_SMOOTH_CEIL
#define RAMP_DISCRETE_STEPS 7

// safe limit ~67% power / ~1200 lm (can sustain 900 lm)
#undef SIMPLE_UI_FLOOR
#undef SIMPLE_UI_CEIL
#define SIMPLE_UI_FLOOR RAMP_DISCRETE_FLOOR
#define SIMPLE_UI_CEIL RAMP_DISCRETE_CEIL


// make candle mode wobble more
#define CANDLE_AMPLITUDE 32

// slow down party strobe; this driver can't pulse for 1ms or less
// (only needed on no-FET build)
#define PARTY_STROBE_ONTIME 2

// stop panicking at ~90% power or ~1600 lm
#undef THERM_FASTER_LEVEL
#define THERM_FASTER_LEVEL 230
#undef MIN_THERM_STEPDOWN
#define MIN_THERM_STEPDOWN DEFAULT_LEVEL

#define USE_UNLOCK_TO_OFF

// disable unused strobe modes
#undef USE_BIKE_FLASHER_MODE
#undef USE_PARTY_STROBE_MODE
#undef USE_TACTICAL_STROBE_MODE