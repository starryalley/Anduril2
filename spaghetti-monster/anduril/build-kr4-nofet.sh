#!/bin/sh

UI=anduril

date '+#define VERSION_NUMBER "%Y%m%d"' > version.h

TARGET=cfg-noctigon-kr4-nofet.h

mkdir -p hex

NAME=$(echo "$TARGET" | perl -ne '/cfg-(.*).h/ && print "$1\n";')
echo "===== $NAME ====="
ATTINY=$(grep 'ATTINY:' $TARGET | awk '{ print $3 }')
if [ -z "$ATTINY" ]; then ATTINY=85 ; fi
echo ../../bin/build.sh $ATTINY "$UI" "-DCONFIGFILE=${TARGET}"
../../bin/build.sh $ATTINY "$UI" "-DCONFIGFILE=${TARGET}"
mv -f "$UI".hex "$UI".$NAME.hex

rm *.elf *.o
mv *.hex hex
