// Noctigon KR4 (fetless) config options for Anduril
// (and Noctigon KR1)
// (and Emisar D4v2 E21A, a.k.a. "D4v2.5")
#include "cfg-noctigon-kr4.h"
#undef MODEL_NUMBER
#define MODEL_NUMBER "0212"
// ATTINY: 1634

// brightness w/ SST-20 4000K LEDs:
// 0/1023: 0.35 lm
// 1/1023: 2.56 lm
// max regulated: 1740 lm
#undef PWM_CHANNELS
#define PWM_CHANNELS 1
#undef RAMP_LENGTH
#define RAMP_LENGTH 200
// prioritize low lows, at risk of visible ripple
// level_calc.py 5.01 1 199 7135 1 0.2 1740 --pwm dyn:78:8191:255
#undef PWM1_LEVELS
#define PWM1_LEVELS 0,1,1,1,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,5,5,5,6,6,7,7,8,8,9,9,10,10,11,12,12,13,13,14,15,15,16,17,17,18,19,19,20,20,21,22,22,23,23,24,24,25,25,26,26,26,26,26,26,26,26,26,26,25,25,24,23,22,21,20,18,16,15,13,10,8,9,9,9,10,10,10,11,11,12,12,13,13,13,14,14,15,15,16,17,17,18,18,19,20,20,21,22,23,23,24,25,26,27,27,28,29,30,31,32,33,34,35,36,37,39,40,41,42,44,45,46,48,49,50,52,53,55,56,58,60,61,63,65,67,69,70,72,74,76,78,80,83,85,87,89,92,94,96,99,101,104,107,109,112,115,118,121,124,127,130,133,136,139,143,146,150,153,157,160,164,168,172,176,180,184,188,192,197,201,206,210,215,220,224,229,234,239,244,250,255
#undef PWM2_LEVELS
#undef PWM_TOPS
#define PWM_TOPS 8191,8191,7576,6934,6265,5567,4841,4086,7041,6178,5287,4368,6995,6279,5546,4797,6356,5721,5075,6080,5510,4934,5645,5124,5615,5137,5488,5044,5300,4887,5077,4691,4832,4921,4578,4642,4320,4365,4384,4097,4106,4097,3840,3827,3802,3573,3547,3336,3309,3274,3088,3054,2884,2851,2696,2663,2522,2488,2360,2239,2126,2019,1918,1824,1736,1652,1573,1439,1371,1254,1144,1043,948,860,736,621,554,455,327,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255

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

// jump start a bit higher than base driver
#undef JUMP_START_MOON
#define JUMP_START_MOON 31

// stop panicking at ~1300 lm
#undef THERM_FASTER_LEVEL
#define THERM_FASTER_LEVEL 180
#undef MIN_THERM_STEPDOWN
#define MIN_THERM_STEPDOWN DEFAULT_LEVEL
