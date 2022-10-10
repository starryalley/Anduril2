#!/bin/bash

if [ -z "$1" ]; then
  echo "Please specify hex file to flash"
  exit 1
fi

avrdude -p attiny1616 -c serialupdi -P /dev/tty.usbserial-0001 -Uflash:w:$1
