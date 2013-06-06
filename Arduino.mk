
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	# linux config
	ARDUINO_DIR = /opt/arduino-1.0
	ARDUINO_PORT = /dev/ttyACM0
	AVR_TOOLS_PATH   = /usr/bin
	AVRDUDE_CONF     = /etc/avrdude.conf
endif

ifeq ($(UNAME_S),Darwin)
	# Mac config

	ARDUINO_DIR = /Applications/Arduino.app/Contents/Resources/Java
	ARDUINO_PORT = /dev/tty.usbmodem1411
	AVR_TOOLS_PATH   = $(ARDUINO_DIR)/hardware/tools/avr/bin
	AVRDUDE_CONF     = $(ARDUINO_DIR)/hardware/tools/avr/etc/avrdude
endif

include ../Arduino-Makefile/arduino-mk/Arduino.mk
