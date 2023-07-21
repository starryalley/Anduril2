# ToyKeeper's Anduril flashlight firmware fork

This is a clone from [ToyKeeper's Flashlight Firmware](https://code.launchpad.net/flashlight-firmware) from branch [anduril2](https://code.launchpad.net/~toykeeper/flashlight-firmware/anduril2), revision [653](https://bazaar.launchpad.net/~toykeeper/flashlight-firmware/anduril2/revision/653)

For ToyKeeper's binary see [here](http://toykeeper.net/torches/fsm/)


# What's here

Since I plan to only work with my existing Anduril2 lights (see below list), I've deleted unrelated FW for other flashlights and many more stuffs from the huge repository. Basically I copied the ToyKeeper/ stuff from the original repo and removed unrelated hwdef and configs here.

This repo contains my own changes to Anduril2 firmware for my several D4v2's, a DW4, a D4K, and some SP10 Pro's and TS10's (as of Jul, 2023). For other Anduril2 lights that I don't own, I won't be able to test or support. Please file a bug report if you see any issue.


## Build targets (which I own):

noctigon-kr4-nofet (`0212`):
- D4v2 brass, 7.5A linear driver with Nichia E21A 2000/2700K mix
- D4v2 antique brass, 4A linear driver with Nichia E17A 1850K

noctigon-kr4 (`0211`):
- D4v2 aluminum, 5A linear driver with Luminus SST-20 4000K
- D4v2 titanium, 5A linear driver with Cree XP-L HI T6 8D 2800K (ramp floor=4, jump_start_level=31)

noctigon-kr4-noaux (`0217`):
- DW4 9A linear driver with 16-LED mule of Nichia E21A 4500/2700 mix

noctigon-kr4-219b (`0214`) (50% FET):
- D4v2 antique brass, 9A linear driver with Nichia 219b SW35
- D4v2 aluminum, 9A linear driver with Nichia 219b SW45k

emisar-d4sv2-tintramp (`0135`): (for D4v2 tintramp)
- D4v2 aluminum, tint ramping 5A+5A linear driver with Nichia 219b 2700/4500K
- D4v2 aluminum, tint ramping ?A+?A linear driver with Nichia E21A 2000/5000K
- D4K tint ramping 10A+10A linear driver with Osram W2 red and W2 green
- D2 with 519A 5000K dedome and 4000K domed

emisar-d1v2-linear-fet (`0124`):
- D1v2, SFN60 6500K, 12A linear with FET

emisar-d1v2-no-fet (`0125`):
- D1v2, W1 green, 5A linear

noctigon-k1-12v (`0253`):
- D1v2, XHP70.3 5000K, 6V 4A boost driver

sofirn-sp10-pro (`0631`):
- Sofirn SP10 Pro AA/14500

wurkkos-ts10 (`0714`):
- Wurkkos TS10 14500

sofirn-lt1s-pro (`0623`): (Not included yet as this is better worked on [Toykeeper's new multi-channel branch](https://bazaar.launchpad.net/~toykeeper/flashlight-firmware/multi-channel/changes) which I haven't got time yet)
- Sofirn LT1S Pro Lantern


## Other build targets (that I don't own):

noctigon-dm11-nofet (`0272`):
- DM11 with w2 deep blue (as requested by [PlaceboConsumer](https://www.reddit.com/user/PlaceboConsumer))

noctigon-dm11-12v (`0273`):
- D4K with boost driver (as confirmed by [RainbowEucalyptus](https://www.reddit.com/user/RainbowEucalyptus))
- KR1 with boost driver (as confirmed by [PlaceboConsumer](https://www.reddit.com/user/PlaceboConsumer))

wurkkos-ts25 (`0715`):
- TS11/TS25 (as confirmed in [this issue](https://github.com/starryalley/Anduril2/issues/24))


Since I've made use of AUX light a lot in this Anduril2 fork, some functions become dependent on AUX and won't work on lights that do not have AUX. So for mule or lights that do not have AUX (for example, KR1 and D1V2), I have added the following in `MODELS` that has `NO_AUX` defined in the config header:

- emisar-d4sv2-tintramp-noaux (0137)
- emisar-d4sv2-tintramp-fet-noaux (0138)
- noctigon-kr4-noaux (0217)
- noctigon-kr4-nofet-noaux (0218)
- noctigon-kr4-219-noaux (0219)
- noctigon-kr4-219b-noaux (0220)
- noctigon-dm11-12v-noaux (0275)

Use this build instead so the following features/changes is ineffective/reverted:
- Blink AUX green LED when powered on - blink main emitters instead
- Use AUX LED to blink numbers (voltage/temperature etc) - blink main emitters instead
- `6C` when in normal mode - will skip "main emitters on, aux high, button low" mode
- `7C` in candle mode to turn on AUX along with main emitters


# Features

Disclaimer: Use at your own risk. I'm not responsible for any loss caused by these firmware changes.

## AUX LED can show different colours based on ambient temperature in standby/lockout mode

Add an additional mode "temperature" in AUX LED mode (for standby/lockout) after "voltage". The AUX LED will change colour based on the on-chip temperature reading. 
  - `<=12C`: pink/purple (red+blue)
  - `12~16C`: blue
  - `16-20C`: cyan (green+blue)
  - `20-25C`: green
  - `25-28C`: yellow (red+green)
  - `>28C`: red

This setting is after "voltage" mode. Use `7H` from off to loop each mode.


## Allow the use of AUX/Indicator LED in lower levels (< DEFAULT_LEVEL) level (6C/6H while light is on)

When in lower levels (< DEFAULT_LEVEL), optionally we can turn on AUX LED along with main emitters.

`6C`: in lower levels, allow AUX colour LED to be turned on along with main emitters for possible tint mix. `6H` changes AUX colour from RED to WHITE (total 7 colours). 

`6C`: in lower levels, cycle through these additional states (only for AUX LEDs):
  - main emitters on, aux high, button low (allow use of AUX LEDs with main emitters)
  - main emitters off, aux off, button high
  - main emitters off, aux off, button low (lowest possible/useful light)
  - back to default (main emitters on, aux off, button low)

No extra state is defined and the AUX LED on is temporary (not remembered) so if there is any button event (ramp up for example), AUX LED will be off. If later user enters moonlight, only main emitters will be lit (default). You need to re-enable this through `6C` or `6H`. The current AUX LED colour is remembered until reboot(factory reset).

When AUX isn't available but indicator LED is:

`6C`: turn on indicator LED
`6H`: turn off indicator LED

Note that in newer Hank lights where there is a RGB switch/button LED, if the light comes with front-facing AUX LED, the RGB switch/button LED is wired to the AUX LED so the button LED control (second and third state in `6C`) won't work. This is not a firmware or hardware issue, but rather expected due to how it is. 


## Blink AUX green LED when powered on (if there is AUX)

Instead of blinking the main emitter, blink the AUX green LED at high power to indicate it's powered on.
Original idea from [here](https://bazaar.launchpad.net/~dnelson1901/flashlight-firmware/flashlight-firmware/revision/267)


## Use AUX LED (red to be specific) or Indicator LED when blinking numbers (For showing voltage/temperature etc)

It's a bit too harsh when using the main emitters to blink voltage/temperature readout. Use red AUX LED instead. For lights with indicator LED only (TS10 for example), the indicator LED (high) will be used.


## Strobe mode can cycle back to the previous state by 3C

`2C` is to cycle to the next strobe state. Add `3C` to cycle back to the previous state. The purpose of this is that the most used strobe states are candle and lightning mode. If I'm already at lightning mode but want to go to candle mode, there was no option but to go through the blinding tactical strobe or party mode. While I don't want to disable tactical strobe nor party mode, let's add an option to just go back to the previous state.


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


## Allow fine ramping up (smallest increment of brightness) using 3C in ramp mode

Ramp up main emitters output in 1 level using `3C`.

Use `9C` to switch between smooth and discrete ramp style instead. (not being used often so make it harder to reach)


## When it's time to change battery, blink AUX red (or indicator led) in standby/lockout mode

When voltage is < 3.2V when light is off, the off/lockout AUX mode will not activate. Instead, the AUX red LED will blink. Original idea from reddit user [connorkmiec93](https://www.reddit.com/user/connorkmiec93/) at this [post](https://www.reddit.com/r/flashlight/comments/mpj36p/im_doing_a_d4v2_with_anduril_2_giveaway/).

The blinking is in a pattern called breathing (although it's only low/high mode in AUX LED) so it's less distracting to the original blinking and one can tell the difference to the normal blinking in RED mode.

For lights with the indicator LED, the indicator LED will be used.

Note that a similar feature is now implemented in [r654](https://bazaar.launchpad.net/~toykeeper/flashlight-firmware/anduril2/revision/654) that is not yet integrated into this fork as of now.

## Fireplace mode (in Candle mode)

2 additional wobble styles: fireplace_slow and fireplace_fast. Use `4H` to cycle through all 3 wobble styles. This is a saved configuration.

- style 1: Anduril2 stock candle mode (default)
- style 2: fireplace slow wobble
- style 3: fireplace fast wobble

In candle wobble style (default/stock) we can additional use `7C` to toggle if we want to use aux led to assist in tint mixing (if AUX is available). Red or yellow aux LED will light up along with the wobbling light.


## Fireworks mode

An additional strobe mode called Fireworks, right after Lightning mode. The main emitters will light up like fireworks. 

Adjust firework brightness (explosion brightness):
- `4C`: decrease brightness by 12
- `5C`: increase brightness by 12
- `6C`: reset brightness to default (max regulated level, which is MAX_1x7135)

For dual channel lights, fireworks mode will randomly choose from the following tint modes when doing the strobes:
- alternate between channel 1 and 2 when 'exploding'
- use channel 1 only
- use channel 2 only
- randomly choose a tint

## Lighthouse mode

An additional strobe mode called Lighthouse beacon, right after Fireworks mode. The main emitters will light up periodically like a lighthouse where the intensity rapidly accelerates to turbo (level 150) and then ramps back down to 0 (when it rotates away from the viewer). It will wait for a few seconds (configurable) before doing it again.

Adjust the delay by:
- `4C`: decrease delay by 1 second (Min: 0 second)
- `5C`: increase delay by 1 second (Max: 30 seconds)
- `6C`: reset delay to default (5 seconds)


## Bad Fluorescent mode

An additional strobe mode called bad fluorescent, right after Lighthouse beacon mode. The main emitters will flicker continously that simulates a broken fluorescent light. Brightness of the flicker can be adjusted by the usual `1H` and `2H` and is remembered.


## 8H to middle tint, 5C to tint edge

In a tint ramping light, use `8H` to go to middle tint. (Copied from [4h to go to middle, not in the middle of 3h](https://github.com/mkong1/anduril/pull/34) )

In addition, there is a shortcut `5C` to go to tint edge. Note that `5C` is for controlling parameters in strobe mode so this shortcut won't work in candle/lightning/firework mode.

Note that when lights are off or locked out, `5C` and `8H` still works but not visible until the emitters are turned on.


## 2 More indicator LED modes (currently only for TS10 which uses indicator LED as single-colour AUX)

Idea from [SammysHP's commit](https://github.com/SammysHP/flashlight-firmware/commits/more-aux-patterns)

Add mode 4 and mode 5 for low/high blinking.

Modes:
- 0: off
- 1: low
- 2: high
- 3: blinking (stock Anduril2)
- 4: blinking low
- 5: blinking high
- 6: breathing (user can't select this. Only used for safer low voltage warning)


## Start Up Mode

Idea from [this post](https://www.reddit.com/r/flashlight/comments/w417mx/anduril_mode_to_ramp_on_for_05_seconds_or_so/).


When doing 1-click from off to turn on the light, there are additionally 3 modes that can be selected through Misc Config Menu (`9H` from off).

- Mode 0: instant on (default in stock Anduril2)
- Mode 1: quickly ramp up to memorized level (default in this fork)
- Mode 2: old fluorescent light flickering
- Mode 3: lighter flickering and slowly ramp up

See [this youtube video](https://www.youtube.com/watch?v=uP6s6mgOBio) for demo.

The Misc Config menu has the following items:
- `USE_TINT_RAMPING` (if enabled) is the first item (for tint ramping lights)
- `USE_JUMP_START` (if enabled) is the second item (for lights with jump start enabled)
- This Start up mode is the third item

Based on different light's config this can be 1st, 2nd or 3rd item in the menu. For dual channel with jump start, it is the third. For single channel with jump start, it is the second. For SP10 pro and TS10 it's the first and only item.

Please note that when start up mode is not 0 (instant on), jump start is disabled because it doesn't make sense to have jump start.


## Smooth sunset from SammysHP

Direct copy from [SammysHP](https://github.com/SammysHP)'s [Smooth sunset v2 patches](https://github.com/SammysHP/flashlight-firmware/commits/smooth-sunset-v2) or [smooth sunset](https://github.com/SammysHP/flashlight-firmware/tree/smooth-sunset) which is awesome.


## Stepped tint ramping from SammysHP

Direct copy from [SammysHP](https://github.com/SammysHP)'s [stepped tint ramping](https://github.com/SammysHP/flashlight-firmware/tree/stepped-tint-ramping). Brilliant work there.


## Tint Alternating Strobe mode

A tint alternating strobe mode (after tactical strobe) for dual channel lights. It switches between channel 1 and 2 continously with configurable brightness (the usual `1H` and `2H`) and interval (`4C` for decreasing and `5C` for increasing the interval by 0.5 second). The setting (brightness and interval) is saved.


## Tint Smooth Ramping Strobe mode

A tint ramping strobe mode (after tint alternating strobe) for dual channel lights. It ramps between both channels repeatedly with configurable brightness (the usual `1H` and `2H`) and pause at each tint step (`4C` for decreasing and `5C` for increasing the interval by 2 microseconds). The setting (brightness and pause) is saved.


## Child Mode (a Simple UI with limited brightness)

There is an additional UI (in addition to Simple and Advanced) that is basically the same as Simple UI but with the following differences (limitations):
- ramping style is fixed to 2-step discrete ramping (only low and high)
- only ramp floor and ceil (basically low and high level) can be configured (by `12H` from off in advanced UI)
- default floor and ceiling level: `RAMP_DISCRETE_FLOOR` and 30

Enter by `12C` from off, exit by `12H` from off.

Switch to this mode before handling the light to the kids.


# Configuration changes

- Default in Advanced mode
- Reordering strobe mode: Candle -> Lightning Storm -> Fireworks -> Bike Flasher -> Party Strobe -> Tactical Strobe -> Tint Alternating -> Tint Smooth Ramping (Candle and Lightning storm are my most used strobe modes, hence why)
- Use `8C` instead of `5C` for momentary mode (not used often so make it harder to enter)
- Use `9C` in ramp mode to switch ramp style (smooth or discrete)
- disable BLINK_AT_RAMP_CEIL/MIDDLE (no blink when ramping to middle or ceiling)
- Lower default candle mode amplitude from 32 to 28 so it is a calmer candle light
- Lower default lightning mode, max possible interval from around 8sec to 16sec so it will appear less busy


# Planned modification/features


## Low power (night) mode

11C/11H to go to this mode and back to advanced mode. 

In this mode:
- default brightness in candle/lightning mode is lowered to min
- max ramp brightness can be only 5 or 10 with button LED configurable off/low/high
- moonlight can be just lighting up button LED on low or high. Trust me, on high level my amber button LED alone is more than enough in pitch black. (I already have a mode which only turns on button LED on high).
- The next level after moonlight can be red AUX on high, and then goes yellow on high, white on high, and then the real level 1 on main emitters. (according to my visual comparison with my zebralight, the level 1 on my E21A 2700/2000mix is around 0.1~0.2lm which is still too bright for me).
- easier control of button LED and AUX LED in this mode
- disable some other settings which can possibly blind dark adapted eyes, for example, only allow candle mode in strobe.


# Other useful branch/commits

- [allow turbo in momentary mode](https://github.com/SammysHP/flashlight-firmware/tree/momentary-turbo)
- [accurate 16-bit PWM strobe](https://bazaar.launchpad.net/~i-dan-3/flashlight-firmware/pwm-strobe/revision/238)
- [beacon tower mode](https://budgetlightforum.com/comment/1727142#comment-1727142)
- [Gabriel's Wurkkos TS10 LVP fix](https://code.launchpad.net/~gabe/flashlight-firmware/anduril2): see rev 618,619

# Development environment

Refer to [a great guide here](https://oweban.org/files/)