CC = xc8-cc

ARGS = -mcpu=12F683\
       -std=c99

INCLUDES = -I .

SRC = dimmer.c spi.c main.c

OUTPUT = -o main.hex

all: compile install

compile:
	$(CC) $(ARGS) $(INCLUDES) $(SRC) $(OUTPUT)

install:
	op -d 12F683 -w main.hex -err 1 | grep -vz interrupted
