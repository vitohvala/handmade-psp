#!/bin/bash

if command -v psp-gcc &> /dev/null; then
    echo "psp-gcc postoji"
else
    echo "podesid path za psp-gcc"
    export PATH=$PATH:/home/vito/psp/pspdev/bin
fi

BINDIR='build'
CC='psp-gcc'
TARGET='code/psp_handmade.c'
BIN=$BINDIR'/handmade.elf'
FLAGS='-Wall'
LIBS='-lpspdebug -lpspdisplay -lpspge -lpspctrl -lpspgu'
INCDIR='-I. -I./../psp/include -I./../psp/sdk/include'
LIBDIR='-L. -L./../psp/lib -L./../psp/sdk/lib'
LDFLAGS='-Wl,-zmax-page-size=128'

if [ ! -d $BINDIR ]; then
    mkdir $BINDIR
fi

$CC $CFLAGS $INCDIR $TARGET -o $BIN $LIBDIR $LIBS $LDFLAGS
    
psp-fixup-imports $BIN    
psp-strip $BIN -o $BINDIR/_strip.elf
    
mksfo "Handmade Hero" $BINDIR/PARAM.SFO
    
pack-pbp $BINDIR/EBOOT.PBP $BINDIR/PARAM.SFO NULL \
            NULL NULL NULL \
            NULL $BINDIR/_strip.elf NULL
    
rm $BINDIR/_strip.elf
