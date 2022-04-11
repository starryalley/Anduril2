#!/bin/bash

if [ -z "$1" ]; then
  echo "Please specify hex file to flash"
  exit 1
fi

avrdude -c usbasp -p t1634 -u -Uflash:w:$1
