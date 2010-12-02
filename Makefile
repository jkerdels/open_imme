# Copyright 2010 Jochen Kerdels, original template by Travis Goodspeed
# and Michael Ossmann
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

TARGET = main

libs = imme.rel tinyfont.rel fahrplan_data.rel imme_font.rel
#libs = 
CC = sdcc
CFLAGS = --no-pack-iram 
#LFLAGS = --xram-loc 0xF000
LFLAGS = --xram-loc 0xF000 --code-size 0x8000 --xram-size 0x0F00
#CFLAGS = -Iinc --debug  --no-pack-iram
#LFLAGS = 

all: $(TARGET).hex

%.rel : %.c
	$(CC) $(CFLAGS) -c $<

main.hex: $(TARGET).rel $(libs)
	sdcc $(LFLAGS) $(TARGET).rel $(libs)
	packihx <$(TARGET).ihx >$(TARGET).hex

clean:
	rm -f *.hex *.ihx *.rel *.asm *.lst *.rst *.sym *.lnk *.map *.mem
