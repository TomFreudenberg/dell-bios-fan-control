#!/usr/bin/make -f

all: dell-bios-fan-control

dell-bios-fan-control: dell-bios-fan-control.c
	$(CC) -o $@ $^

clean:
	rm -f dell-bios-fan-control
