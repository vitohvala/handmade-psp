#!/bin/bash

if command -v psp-gcc &> /dev/null; then
    echo "psp-gcc exists"
else
    echo "psp-gcc not in your PATH"
    exit 1
    //export PATH=$PATH:/home/vito/psp/pspdev/bin
fi

PSPSDK_PATH=$(psp-config --pspsdk-path)
PSPDEV_PATH=$(psp-config --pspdev-path)
PSP_PREFIX=$(psp-config --psp-prefix) #

BINDIR='build'
CC='psp-gcc'
TARGET='code/psp_handmade.c'
BIN=$BINDIR'/handmade.elf'
FLAGS='-Wall'
LIBS='-lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspgu -lpspaudiolib -lpspaudio'
INCDIR="-I. -I$PSPDEV_PATH/include -I$PSPSDK_PATH/include"
LIBDIR="-L. -L$PSPDEV_PATH/lib -L$PSPSDK_PATH/lib"
LDFLAGS='-Wl,-zmax-page-size=128'

if [ ! -d $BINDIR ]; then
    mkdir $BINDIR
fi

#set -xe
$CC $CFLAGS $INCDIR $TARGET -o $BIN $LIBDIR $LIBS $LDFLAGS

psp-fixup-imports $BIN
psp-strip $BIN -o $BINDIR/_strip.elf

mksfo "Handmade Hero" $BINDIR/PARAM.SFO

pack-pbp $BINDIR/EBOOT.PBP $BINDIR/PARAM.SFO NULL \
            NULL NULL NULL \
            NULL $BINDIR/_strip.elf NULL

rm $BINDIR/_strip.elf
