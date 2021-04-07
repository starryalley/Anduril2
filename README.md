# ToyKeeper's Anduril flashlight firmware fork

This is a clone from [ToyKeeper's Flashlight Firmware](https://code.launchpad.net/flashlight-firmware) from branch [anduril2](https://code.launchpad.net/~toykeeper/flashlight-firmware/anduril2), revision [583](https://bazaar.launchpad.net/~toykeeper/flashlight-firmware/anduril2/revision/583)

For toyKeeper's binary see [here](http://toykeeper.net/torches/fsm/)


# What's here

Since I plan to only work with my Emisar D4v2, I've deleted unrelated FW for other flashlights and many more stuffs from the huge repository. Basically I copied the ToyKeeper/ stuff from the original repo and removed unrelated hwdef and configs there.

This repo contains my own changes to Anduril2 firmware on my Bbrass Emisar D4v2. (7.5A KR4 nofet driver with E21A 2000/2700K mix, amber button LED).

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

If temperature reading (standby or lockout) is out of comfort zone (`18-25C`), button LED will blink (if not in blinking AUX LED mode).


## Allow the use of AUX LED in moonlight (lowest) level (5C/6C while light is on)

When in moon mode (lowest level), optionally we can turn on AUX LED along with main emitters.

5C: in lowest level (ramp floor), allow AUX colour LED to be turned on along with main emitters for possible tint mix. 5C changes AUX colour from RED to WHITE (total 7 colours).
    
6C: in lowest level, cycle through these additional states:
  - main emitters on, aux high, button low
  - main emitters off, aux high, button low
  - main emitters off, aux off, button high (lowest possible/useful light)
  - back to default (main emitters on, aux off, button low)

No extra state is defined and the AUX LED on is temporary (not remembered) so if there is any button event (ramp up for example), AUX LED will be off. If later user enters moonlight, only main emitters will be lit (default). You need to re-enable this through 5C or 6C. The current AUX LED colour is remembered until reboot(factory reset).

## Blink AUX green LED when powered on

Instead of blinking the main emitter, blink the AUX green LED at high power to indicate it's powered on.
Original idea from [here](https://bazaar.launchpad.net/~dnelson1901/flashlight-firmware/flashlight-firmware/revision/267)

# Configuration changes

- Smooth floor level set to 1 (lowest but unstable moonlight)
- Default level set to smooth floor level (moonlight)
- Simple UI is inactive by default (Advanced UI is the default now)
- Reording strobe mode: Candle->Lightning Storm->Bike Flasher->Party Strobe->Tactical Strobe. (Candle and Lightning storm are my most used strobe modes, hence why)
- Disable Biker Flasher (I'm not using D4v2 for biking)
- Use 8C instead of 5C for momentary mode (not used often so make it harder to enter)
- Default AUX LED mode in standby/lockout mode set to show current temperature with low/high setting respectively

# Other changes to the codebase

- Define PWM1_TOP so we can modify PWM cycles if needed (currently using 10-bit PWM on KR4-nofet firmware). Original idea from [16-bit PWM dimmer moonlight on d4v2](https://bazaar.launchpad.net/~dnelson1901/flashlight-firmware/flashlight-firmware/revision/278)
- in standby (battery connected, main emitters off) both voltage and temperature (instead of just voltage) is updated regularly. This allows UI to read up-to-date temperature as well as voltage.

# Planned modification/features

## Breathing button LED during standby/lock

Breathing button LED during standby/lockout. But the button LED only has 3 levels (off, low, high) so not sure if it's possible to simulate the pulsing effect.

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