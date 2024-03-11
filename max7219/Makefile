
BAUD = 9600UL
F_CPU = 16000000UL
MCU = atmega328p
PROGRAMMER = arduino
PORT = /dev/tty.usbmodem621


CPP=avr-g++
CPP_INCLUDES = -I.
CPP_FLAGS = -c -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD)
CPP_FLAGS += -Os -g -std=c++14 -Wall
CPP_FLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CPP_FLAGS += -ffunction-sections -fdata-sections -ffreestanding


.PHONY: all clean flash


all: main.hex size

# Compile
main.o: main.cpp
	$(CPP) $(CPP_FLAGS) $(CPP_INCLUDES) -o main.o main.cpp

max7219.o: max7219.h max7219.cpp
	$(CPP) $(CPP_FLAGS) $(CPP_INCLUDES) -o max7219.o max7219.cpp

xoroshiro64.o: xoroshiro64.h xoroshiro64.cpp
	$(CPP) $(CPP_FLAGS) $(CPP_INCLUDES) -o xoroshiro64.o xoroshiro64.cpp


# Link
main.elf: main.o max7219.o xoroshiro64.o
	$(CPP) -Wl,--gc-sections -I. -mmcu=$(MCU) main.o max7219.o xoroshiro64.o -o main.elf


main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex


clean:
	rm -f *.o *.elf *.hex *.s


flash: main.hex
	avrdude -c $(PROGRAMMER) -P $(PORT) -p $(MCU) -U flash:w:main.hex


size: main.elf
	@echo $(MCU)
	avr-size main.elf
