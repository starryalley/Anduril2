# ToyKeeper's Anduril flashlight firmware fork

This is a clone from [ToyKeeper's Flashlight Firmware](https://code.launchpad.net/flashlight-firmware) from branch [anduril2](https://code.launchpad.net/~toykeeper/flashlight-firmware/anduril2), revision [583](https://bazaar.launchpad.net/~toykeeper/flashlight-firmware/anduril2/revision/583)

For toyKeeper's binary see [here](http://toykeeper.net/torches/fsm/)


# What's here

Since I plan to only work with my Emisar D4v2, I've deleted unrelated FW for other flashlights and many more stuffs from the huge repository. Basically I copied the ToyKeeper/ stuff from the original repo and removed unrelated hwdef and configs there.

This repo contains my own changes to Anduril2 firmware on my brass Emisar D4v2. (7.5A KR4 nofet driver with E21A 2000/2700K mix, amber button LED).

# Features

Warning: Use at your own risk. I'm not responsible for any loss caused by these firmware changes.

## AUX LED can show different colours based on ambient temperature in standby/lockout mode

Add an additional mode "temperature" in AUX LED mode (for standby/lockout) after "voltage". The AUX LED will change colour based on the on-chip temperature reading. 
  - `<=12C`: pink/purple (red+blue)
  - `12~16C`: blue
  - `16-20C`: cyan (green+blue)
  - `20-25C`: green
  - `25-28C`: yellow (red+green)
  - `>28C`: red

 Default AUX LED OFF mode in standby is set to "low" and "temperature", and the mode in lockout is set to "high" and "temperature". 

## Button LED blinking if ambient temperature is out of comfort zone

If temperature reading (standby or lockout) is out of comfort zone (`18-25C`), button LED will blink (if not in blinking AUX LED mode). Use 6C in standby or lockout mode to toggle the setting to blink button LED or not. This configuration is saved.


## Allow the use of AUX LED in moonlight (lowest) level (6C/6H while light is on)

When in moon mode (lowest level), optionally we can turn on AUX LED along with main emitters.

6C: in lowest level (ramp floor), allow AUX colour LED to be turned on along with main emitters for possible tint mix. 6H changes AUX colour from RED to WHITE (total 7 colours).
    
6C: in lowest level, cycle through these additional states:
  - main emitters off, aux off, button high (lowest possible/useful light)
  - main emitters off, aux high, button low
  - main emitters on, aux high, button low
  - back to default (main emitters on, aux off, button low)

No extra state is defined and the AUX LED on is temporary (not remembered) so if there is any button event (ramp up for example), AUX LED will be off. If later user enters moonlight, only main emitters will be lit (default). You need to re-enable this through 6C or 6H. The current AUX LED colour is remembered until reboot(factory reset).

## Blink AUX green LED when powered on

Instead of blinking the main emitter, blink the AUX green LED at high power to indicate it's powered on.
Original idea from [here](https://bazaar.launchpad.net/~dnelson1901/flashlight-firmware/flashlight-firmware/revision/267)

## Make blinking brightness 2-stage

When main emitters are blinking for showing information (such as voltage or temperature), the blinking brightness has 2 stage: high or low. When memorised level is below 25, blinking brightness will be low (around level 6). Otherwise it is default (around 23) In case we want to check temperature during the middle of the night, the low blinking brightness helps.

## Strobe mode can cycle back to the previous state by 3C

2C is to cycle to the next strobe state. Add 3C to cycle back to the previous state. The purpose of this is that the most used strobe states are candle and lightning mode. If I'm already at lightning mode but want to go to candle mode, there was no option but to go through the blinding tactical strobe or party mode. While I don't want to disable tactical strobe nor party mode, let's add an option to just go back to the previous state.

## More configurable lightning mode 

In lightning mode, add the following options which are saved:
- `4C`: turn down busy factor so the lightning is less busy (less frequent)
- `5C`: turn up busy factor so the lightning is busier (more frequent)
- `6C`: reset to default (which is max 16sec until the next lightning strike, originally it's about 8sec)

## More configurable candle mode

In candle mode, add the following options which are saved:
- `4C`: making candle amplitude smaller (candle in sort of a more stillness, calmer)
- `5C`: making candle amplitude bigger (candle in the wind)
- `6C`: reset to default (which is a bit calmer then the stock one)

## When in lockout mode, unlock will go to off state

In Anduril2 lockout state will always unlock to steady state (on state). Modify it so that unlocking will go to off state instead. Original idea from [here](https://github.com/mkong1/anduril/pull/13/files)

## Allow fine ramping up (smallest increment of brightness) using 3C in ramp mode

Ramp up main emitters output in smallest increment using `3C`. This doesn't get saved and when use 1H to ramp up/down it will just revert to the original level and ramp up/down from there. 

Use `9C` to switch between smooth and discrete ramp style. Not being used often by me so make it harder to reach.


## When it's time to change battery, blink red in standby/lockout mode

When voltage is <3.4V, the original off/lockout AUX mode will not activate. Instead, the AUX red LED will blink. Original idea from reddit user [connorkmiec93](https://www.reddit.com/user/connorkmiec93/) at this [post](https://www.reddit.com/r/flashlight/comments/mpj36p/im_doing_a_d4v2_with_anduril_2_giveaway/).

The blinking is in a pattern called breathing (although it's only low/high mode in AUX LED) so it's less distracting to the original blinking and one can tell the difference to the normal blinking in RED mode.


## Fireplace mode (in Candle mode)

2 additional wobble styles: fireplace_slow and fireplace_fast. Use 3H to cycle through all 3 wobble styles. This is a saved configuration. 

In candle wobble style (default/stock) we can additional use 7C to toggle if we want to use aux led to assist in tint mixing. Red or yellow aux LED will light up along with the wobbling light.


# Configuration changes

- Smooth floor level set to 1 (lowest but unstable moonlight)
- Default level set to smooth floor level (moonlight)
- Simple UI is inactive by default (Advanced UI is the default now)
- Reordering strobe mode: Candle->Lightning Storm->Bike Flasher->Party Strobe->Tactical Strobe. (Candle and Lightning storm are my most used strobe modes, hence why)
- Disable Biker Flasher (I'm not using D4v2 for biking)
- Use 8C instead of 5C for momentary mode (not used often so make it harder to enter)
- Default AUX LED mode in standby/lockout mode set to show current temperature with low/high setting respectively
- Lower default candle mode amplitude from 32 to 28 so it is a calmer candle light
- Lower default lightning mode max possible interval from around 8sec to 16sec so it will appear less busy
- Use a quarter of clock speed instead of half, and 1/8 of clock speed insteead of a quarter when in lower levels.
- Use 9C in ramp mode to switch ramp style (smooth or discrete)

# Other changes to the codebase

- Define PWM1_TOP so we can modify PWM cycles if needed (currently using 10-bit PWM on KR4-nofet firmware). Original idea from [16-bit PWM dimmer moonlight on d4v2](https://bazaar.launchpad.net/~dnelson1901/flashlight-firmware/flashlight-firmware/revision/278)
- in standby (battery connected, main emitters off) both voltage and temperature (instead of just voltage) is updated regularly. This allows UI to read up-to-date temperature as well as voltage.

# Planned modification/features

## Improving button LED blinking when temperature is out of comfort zone

- Comfortable zone button blink and work when aux led is set to high. Make it blink by turning off and on instead.
- Comfortable zone button blink can work in moonlight mode as well. 


## Turn the flashlight into a countdown timer

Slowly ramping up the light. When timeout, buzz it. Sort of. Maybe alarm clock as well? But I am not really going to use it anyway.

## Low power (night) mode

11C/11H to go to this mode and back to advanced mode. 

In this mode:
- blink brightness is lowered
- default brightness in candle/lightning mode is lowered to min
- max ramp brightness can be only 5 or 10 with button LED configurable off/low/high
- moonlight can be just lighting up button LED on low or high. Trust me, on high level my amber button LED alone is more than enough in pitch black. (I already have a mode which only turns on button LED on high).
- The next level after moonlight can be red AUX on high, and then goes yellow on high, white on high, and then the real level 1 on main emitters. (according to my visual comparison with my zebralight, the level 1 on my E21A 2700/2000mix is around 0.1~0.2lm which is still too bright for me).
- easier control of button LED and AUX LED in this mode
- disable some other settings which can possibly blind dark adapted eyes.


## Exit momentary mode without physically disconnect?

15C to exit? But I don't use momentary mode at all.

## Fireworks strobe mode

Simulate fireworks (similar to the effect of main emitters when doing factory reset). If we have multiple Anduril lights with different color temperature emitters it would be fun.


# Other useful commits

- [allow turbo in momentary mode](https://github.com/SammysHP/flashlight-firmware/commit/db7d58aa6d3f47336ace646fc6d4818d86c08956)
- [more steps to sunset mode](https://github.com/SammysHP/flashlight-firmware/commit/499489bfbc0d0e7501f30a675e58aaa4c36f432e)
- [fix ramping in sunset mode](https://github.com/SammysHP/flashlight-firmware/commit/27433bf7589bf35c19bb1d2b5a98341ac402d139)
- [accurate 16-bit PWM strobe](https://bazaar.launchpad.net/~i-dan-3/flashlight-firmware/pwm-strobe/revision/238)
- [beacon tower mode](https://budgetlightforum.com/comment/1727142#comment-1727142)

# Usage

I've only included a build script for KR4-nofet (`spaghetti-monster/anduril/build-kr4-nofet.sh`) because that's the only light I own. Modify as needed if you have a different light. 

# Development environment

Refer to [a great guide here](https://oweban.org/files/)