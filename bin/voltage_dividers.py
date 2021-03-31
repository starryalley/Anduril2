#!/usr/bin/env python

from __future__ import print_function

interactive = False


def main(args):
    """Calculates values related to voltage dividers"""

    # Get parameters from the user
    questions_main = [
            (int, 'num_cells', 1, 'How many cells in series?'),
            (float, 'val_r1', 191000, 'R1 value?'),
            (float, 'val_r2', 47000, 'R2 value?'),
            (float, 'val_vref', 1.1, 'MCU reference voltage?'),
            ]

    def ask(questions, ans):
        for typ, name, default, text in questions:
            value = get_value(text, default, args)
            if not value:
                value = default
            else:
                value = typ(value)
            setattr(ans, name, value)

    answers = Empty()
    ask(questions_main, answers)

    # do the math
    low, high = adc_range(answers.num_cells, answers.val_r1,
                          answers.val_r2, answers.val_vref)
    drain = standby(answers.num_cells, answers.val_r1, answers.val_r2)

    if (low < 32) or (high < 32) or \
       (low > 224) or (high > 224):
        print('WARNING: ADC values outside expected range of 32 to 224')
    print('ADC range: %i - %i' % (low, high))
    print('Standby power: ~%.3f mA' % (drain))

    if interactive: # Wait on exit, in case user invoked us by clicking an icon
        print('Press Enter to exit:')
        input_text()


class Empty:
    pass


def standby(s, r1, r2):
    """Calculate approximate parasitic / standby drain in mA """
    # I pulled this formula out of my ass
    #low = (2.8 * s * 2000) / (r1 + r2)
    #hi  = (4.2 * s * 2000) / (r1 + r2)
    avg = (3.6 * s * 2000) / (r1 + r2)
    return avg


def adc_range(s, r1, r2, vref=1.1):
    """Calculate low and high ADC values for the given configuration"""
    low = v2adc(2.8*s, r1, r2, vref)
    hi  = v2adc(4.2*s, r1, r2, vref)
    return low, hi


def v2adc(v, r1, r2, vref=1.1):
    """Calculate expected ADC reading for the given configuration"""
    adc = (v * r2 * 255) / ((r1 + r2) * vref)
    return adc


def get_value(text, default, args):
    """Get input from the user, or from the command line args."""
    if args:
        result = args[0]
        del args[0]
    else:
        global interactive
        interactive = True
        print(text + ' (%s) ' % (default), end='')
        result = input_text()
    result = result.strip()
    return result


def input_text():
    try:
        value = raw_input()  # python2
    except NameError:
        value = input()  # python3
    return value


if __name__ == "__main__":
    import sys
    main(sys.argv[1:])

