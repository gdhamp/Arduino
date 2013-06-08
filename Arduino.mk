
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	# linux config
	ARDUINO_DIR = /opt/arduino
	ARDUINO_PORT = /dev/ttyACM0
	AVR_TOOLS_PATH   = $(ARDUINO_DIR)/hardware/tools/avr/bin
	AVRDUDE_CONF     = $(ARDUINO_DIR)/hardware/tools/avrdude
endif

ifeq ($(UNAME_S),Darwin)
	# Mac config

	ARDUINO_DIR = /Applications/Arduino.app/Contents/Resources/Java
	ARDUINO_PORT = /dev/tty.usbmodem1411
	AVR_TOOLS_PATH   = $(ARDUINO_DIR)/hardware/tools/avr/bin
	AVRDUDE_CONF     = $(ARDUINO_DIR)/hardware/tools/avr/etc/avrdude
endif

