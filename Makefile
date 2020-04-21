# keypad -- Simple keypad to illustrate the use of XmForm widget
#           class.
# Author: Luis Colorado <luiscoloradourcola@gmail.com>
# Copyright: (C) 2020 Luis Colorado.  All rights reserved.
# License: BSD.

         CFLAGS = -I/usr/local/include -g -O0
            RM ?= rm -f

        targets = keypad
        toclean = $(targets)

    keypad_deps =
    keypad_objs = keypad.o
 keypad_ldflags = -L/usr/local/lib
    keypad_libs = -lXm -lXt -lX11
       toclean += $(keypad_objs)

all: $(targets)
clean:
	$(RM) $(toclean)

keypad: $(keypad_deps) $(keypad_objs)
	$(CC) $(CFLAGS) $(LDFLAGS) $($@_ldflags) -o $@ $($@_objs) $($@_libs)
