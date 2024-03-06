
##########------------------------------------------------------##########
##########              Project-specific Details                ##########
##########    Check these every time you start a new project    ##########
##########------------------------------------------------------##########

MCU   = atmega328p
F_CPU = 1000000UL
BAUD  = 9600UL


##########------------------------------------------------------##########
##########                 Programmer Defaults                  ##########
##########------------------------------------------------------##########

PROGRAMMER_TYPE = usbtiny
PROGRAMMER_ARGS =


##########------------------------------------------------------##########
##########                  Program Locations                   ##########
##########------------------------------------------------------##########

AVRA = avra
AVRDUDE = avrdude

##########------------------------------------------------------##########
##########					    Targets 						##########
##########------------------------------------------------------##########

SOURCE = blink.asm
TARGET = blink.hex

$(TARGET): $(SOURCE)
	$(AVRA) $(SOURCE)

.PHONY: all flash

all: $(TARGET)

clean:
	rm -f *.cof *.obj *.hex *.map

flash: $(TARGET)
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$(TARGET)
