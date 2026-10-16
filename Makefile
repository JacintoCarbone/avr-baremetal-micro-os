# Compiler & Toolchain configuration
MCU = atmega328p
F_CPU = 16000000UL
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRDUDE = avrdude

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra -std=c99 -Isrc
LDFLAGS = -mmcu=$(MCU)

SRC = src/main.c src/uart.c src/timer.c
OBJ = $(SRC:.c=.o)
TARGET = firmware

all: $(TARGET).hex $(TARGET).bin

$(TARGET).elf: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^
	avr-size --format=avr --mcu=$(MCU) $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary -R .eeprom $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET).elf $(TARGET).hex $(TARGET).bin

flash: $(TARGET).hex
	$(AVRDUDE) -c arduino -p m328p -P /dev/ttyUSB0 -b 115200 -U flash:w:$<:i

.PHONY: all clean flash
