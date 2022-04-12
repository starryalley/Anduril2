# ToyKeeper's Anduril flashlight firmware fork

This is a clone from [ToyKeeper's Flashlight Firmware](https://code.launchpad.net/flashlight-firmware) from branch [anduril2](https://code.launchpad.net/~toykeeper/flashlight-firmware/anduril2), revision [647](https://bazaar.launchpad.net/~toykeeper/flashlight-firmware/anduril2/revision/647)

For toyKeeper's binary see [here](http://toykeeper.net/torches/fsm/)


# What's here

Since I plan to only work with my Emisar D4v2s, I've deleted unrelated FW for other flashlights and many more stuffs from the huge repository. Basically I copied the ToyKeeper/ stuff from the original repo and removed unrelated hwdef and configs here.

This repo contains my own changes to Anduril2 firmware for my 8 D4v2 (as of Apr 2022)


kr4-nofet:
- brass, 7.5A CC driver with Nichia E21A 2000/2700K mix
- antique brass, unknown driver (probably CC) with Nichia E17A 1850K

kr4:
- aluminum, 5A CC driver with Luminus SST-20 4000K
- titanium, 5A CC driver with Cree XP-L HI T6 8D 2800K

kr4-219b (50% FET):
- antique brass, 9A CC driver with Nichia 219b SW35
- aluminum, 9A CC driver with Nichia 219b SW45k

d4sv2-tintramp: (for D4v2 tintramp)
- aluminum, tintramping with Nichia 219b 2700/4500K
- aluminum, tintramping with Nichia E21A 2000/5000K


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


## Allow the use of AUX LED in lower levels (< DEFAULT_LEVEL) level (6C/6H while light is on)

When in lower levels (< DEFAULT_LEVEL), optionally we can turn on AUX LED along with main emitters.

`6C`: in lowest level (ramp floor), allow AUX colour LED to be turned on along with main emitters for possible tint mix. `6H` changes AUX colour from RED to WHITE (total 7 colours).
    
`6C`: in lowest level, cycle through these additional states:
  - main emitters off, aux off, button high (lowest possible/useful light)
  - main emitters off, aux high, button low
  - main emitters on, aux high, button low
  - back to default (main emitters on, aux off, button low)

No extra state is defined and the AUX LED on is temporary (not remembered) so if there is any button event (ramp up for example), AUX LED will be off. If later user enters moonlight, only main emitters will be lit (default). You need to re-enable this through `6C` or `6H`. The current AUX LED colour is remembered until reboot(factory reset).


## Blink AUX green LED when powered on

Instead of blinking the main emitter, blink the AUX green LED at high power to indicate it's powered on.
Original idea from [here](https://bazaar.launchpad.net/~dnelson1901/flashlight-firmware/flashlight-firmware/revision/267)


## Use AUX LED (red to be specific) when blinking numbers (For showing voltage/temperature etc)

It's a bit too harsh when using the main emitters to blink voltage/temperature readout. Use red AUX LED instead. 


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

Use `9C` to switch between smooth and discrete ramp style. Not being used often by me so make it harder to reach.


## When it's time to change battery, blink red in standby/lockout mode

When voltage is < 3.4V when light is off, the off/lockout AUX mode will not activate. Instead, the AUX red LED will blink. Original idea from reddit user [connorkmiec93](https://www.reddit.com/user/connorkmiec93/) at this [post](https://www.reddit.com/r/flashlight/comments/mpj36p/im_doing_a_d4v2_with_anduril_2_giveaway/).

The blinking is in a pattern called breathing (although it's only low/high mode in AUX LED) so it's less distracting to the original blinking and one can tell the difference to the normal blinking in RED mode.


## Fireplace mode (in Candle mode)

2 additional wobble styles: fireplace_slow and fireplace_fast. Use `4H` to cycle through all 3 wobble styles. This is a saved configuration. 

In candle wobble style (default/stock) we can additional use `7C` to toggle if we want to use aux led to assist in tint mixing. Red or yellow aux LED will light up along with the wobbling light.


## Fireworks mode

There is an additional strobe mode called Fireworks, right after Lightning mode. The main emitters will light up like fireworks. 

Adjust firework brightness (explosion brightness):
- `4C`: decrease brightness by 12
- `5C`: increase brightness by 12
- `6C`: reset brightness to default (max regulated level, which is MAX_1x7135)


# Configuration changes

- Reordering strobe mode: Candle -> Lightning Storm -> Fireworks -> Bike Flasher -> Party Strobe -> Tactical Strobe. (Candle and Lightning storm are my most used strobe modes, hence why)
- Use 8C instead of 5C for momentary mode (not used often so make it harder to enter)
- Use 9C in ramp mode to switch ramp style (smooth or discrete)
- Lower default candle mode amplitude from 32 to 28 so it is a calmer candle light
- Lower default lightning mode max possible interval from around 8sec to 16sec so it will appear less busy


# Planned modification/features


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


# Other useful commits

- [allow turbo in momentary mode](https://github.com/SammysHP/flashlight-firmware/commit/db7d58aa6d3f47336ace646fc6d4818d86c08956)
- [more steps to sunset mode](https://github.com/SammysHP/flashlight-firmware/commit/499489bfbc0d0e7501f30a675e58aaa4c36f432e)
- [fix ramping in sunset mode](https://github.com/SammysHP/flashlight-firmware/commit/27433bf7589bf35c19bb1d2b5a98341ac402d139)
- [accurate 16-bit PWM strobe](https://bazaar.launchpad.net/~i-dan-3/flashlight-firmware/pwm-strobe/revision/238)
- [beacon tower mode](https://budgetlightforum.com/comment/1727142#comment-1727142)


# Development environment

Refer to [a great guide here](https://oweban.org/files/)