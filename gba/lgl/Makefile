# Programs
CROSS		= arm-eabi-
CC		= $(CROSS)gcc
AS		= $(CROSS)as
STRIP		= $(CROSS)strip
OBJCOPY		= $(CROSS)objcopy
AR		= $(CROSS)ar
GBA_HEADER 	= gba-header
IMGCONVERTER    = imgconverter

COMFLAGS	= -mthumb-interwork
CFLAGS 		= -Wall -O2 $(COMFLAGS)
LDFLAGS		= -specs=gba.specs -mthumb-interwork

# Project specific 
LIBRARY		= liblgl.a
OBJECTS		= sprites.o memory.o bg.o fonts_bg.o fonts_sprite.o \
		  posprintf.o debug.o input.o
DATAOBJS	= data_default_font.o

$(LIBRARY): $(OBJECTS) $(DATAOBJS)
	$(AR) rcs $@ $^

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.S.o:
	$(AS) $(COMFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -rf *.a *.o *.elf *.raw
