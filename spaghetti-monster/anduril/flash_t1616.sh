#!/bin/bash

if [ -z "$1" ]; then
  echo "Please specify hex file to flash"
  exit 1
fi

echo "Erasing..."
pymcuprog erase -d attiny1616 -t uart -u /dev/tty.usbserial-0001
echo "Flashing..."
pymcuprog write -d attiny1616 -t uart -u /dev/tty.usbserial-0001 -f $1 --verify
