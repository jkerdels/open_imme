# Object files directory
# Warning: This will be removed by make clean!
#
OBJDIR = obj

# Target file name (without extension)
TARGET = $(OBJDIR)/main

# Define all c source files (dependencies are generated automatically)
#
SOURCES  = main.c
SOURCES += imme.c
SOURCES += fahrplan_data.c
SOURCES += fahrplan.c
SOURCES += tools.c
SOURCES += gfx.c
SOURCES += keys.c


OBJECTS = $(addprefix $(OBJDIR)/,$(addsuffix .rel,$(basename $(SOURCES))))

# Compiler options
#
CFLAGS   = --no-pack-iram

# Linker Options
#
LDFLAGS  = --code-size 0x8000 
LDFLAGS += --xram-loc 0xF000 
LDFLAGS += --xram-size 0x0F00

# TeensyProg Options
#
TEENSY_FLAGS = -P /dev/ttyACM0
#TEENSY_FLAGS = -P /dev/com8   


# Define programs and commands
#
CC           = sdcc
PACKIHX      = packihx
#TEENSY_PROG  = tools/teensy-prog/client/main
TEENSY_PROG  = tools/teensy-prog/client/bin/Debug/immeFlash


# Default target
#
all:  ccversion hex
hex:  $(TARGET).hex


# Create object files directory
#
$(shell mkdir -p $(OBJDIR) 2>/dev/null)


# Display compiler version information
#
ccversion: 
	@$(CC) --version


# Compile: create object files from C source files
#
$(OBJDIR)/%.rel : %.c
	@echo
	@echo Compiling C: $<
	$(CC) $(CFLAGS) -c $< -o $@ 


# Link: create IHX output file from object files
#
.SECONDARY: $(TARGET).ihx
.PRECIOUS:  $(OBJECTS)
$(TARGET).ihx: $(OBJECTS)
	@echo
	@echo Linking: $@
	$(CC) $(LDFLAGS) -o $(TARGET).ihx $^
	#-o $@ 


# Create final output file
#
%.hex: %.ihx
	@echo
	@echo Packing hex file: $@
	$(PACKIHX) <$(TARGET).ihx >$(TARGET).hex


# Program the device
#
flash: $(TARGET).hex
	$(TEENSY_PROG) $(TEENSY_FLAGS) -f $(TARGET).hex


# Clean project
#
clean:
	@echo Cleaning project:
	rm -rf $(OBJDIR)


# Listing of phony targets.
#
.PHONY: all ccversion hex flash clean
