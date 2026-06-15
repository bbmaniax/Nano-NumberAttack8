# Makefile for Arduino application projects
.DEFAULT_GOAL := all

## Project specific configurations

PROJECT ?= Nano-NumberAttack8
SKETCH ?= $(PROJECT).ino
LIBS ?= DigitalButton
CORES ?= \
	arduino:avr \
	MicroCore:avr \
	ATTinyCore:avr

## Project specific targets

.PHONY: build
build: build/nano-old

.PHONY: deploy
deploy: deploy/nano-old

## Configurations

BUILD_CONFIG ?= ./arduino-cli.yaml
BUILD_DIR ?= ./build

DEPLOY_ARDUINO_PORT_TTYUSB ?= /dev/ttyUSB0
DEPLOY_ARDUINO_PORT_TTYACM ?= /dev/ttyACM0

## Macros

define build-arduino
	arduino-cli compile \
		--fqbn $(1) \
		--export-binaries \
		$(if $(filter-out undefined,$(origin DEBUG)),--build-property "build.extra_flags=-DDEBUG") \
		$(SKETCH)
endef

define deploy-arduino
	arduino-cli upload --verbose \
		-b $(1) \
		-p $(2) \
		--input-file $(BUILD_DIR)/$(subst :,.,$(word 1,$(subst :, ,$(1))).$(word 2,$(subst :, ,$(1))).$(word 3,$(subst :, ,$(1))))/$(basename $(3)).ino.hex
endef

define deploy-arduinoasisp
	arduino-cli upload --verbose \
		-b $(1) \
		-p $(2) \
		-P arduinoasisp \
		--input-dir $(BUILD_DIR)/$(subst :,.,$(word 1,$(subst :, ,$(1))).$(word 2,$(subst :, ,$(1))).$(word 3,$(subst :, ,$(1))))
endef

## Targets

.PHONY: clean
clean:
	find . -type d -name "build" -exec rm -rf {} +
	find . -type f -name "*.lst" -exec rm {} +
	find . -type f -name "*.map" -exec rm {} +
	find . -type f -name "*.gcda" -exec rm {} +
	find . -type f -name "*.gcno" -exec rm {} +
	find . -type f -exec chmod -x {} +

.PHONY: all
all: clean install build

.PHONY: install
install: install/core install/lib

.PHONY: install/core
install/core:
ifeq ($(strip $(CORES)),)
	@echo "No cores defined. Skipping install/core."
else
	@echo "Installing Arduino cores..."
	@if [ ! -f ~/.arduino15/arduino-cli.yaml ]; then arduino-cli config init; fi
	arduino-cli --config-file $(BUILD_CONFIG) core update-index
	arduino-cli --config-file $(BUILD_CONFIG) core install $(CORES)
	@echo ""
	@echo "Arduino cores installed."
endif

.PHONY: install/lib
install/lib:
ifeq ($(strip $(LIBS)),)
	@echo "No libraries defined. Skipping install/lib."
else
	@echo "Installing Arduino libraries..."
	@if [ ! -f ~/.arduino15/arduino-cli.yaml ]; then arduino-cli config init; fi
	arduino-cli --config-file $(BUILD_CONFIG) lib update-index
	arduino-cli --config-file $(BUILD_CONFIG) lib install $(LIBS)
	@echo ""
	@echo "Arduino libraries installed."
endif

## Build targets for each board

.PHONY: build/nano
build/nano:
	$(call build-arduino,arduino:avr:nano:cpu=atmega328)

.PHONY: build/nano-old
build/nano-old:
	$(call build-arduino,arduino:avr:nano:cpu=atmega328old)

.PHONY: build/attiny13a
build/attiny13a:
	$(call build-arduino,MicroCore:avr:13:clock=1M2)

.PHONY: build/attiny45
build/attiny45:
	$(call build-arduino,ATTinyCore:avr:attinyx5:chip=45)

.PHONY: build/attiny85
build/attiny85:
	$(call build-arduino,ATTinyCore:avr:attinyx5:chip=85)

## Deploy targets for each board

.PHONY: deploy/nano
deploy/nano:
	$(call deploy-arduino,arduino:avr:nano:cpu=atmega328,$(DEPLOY_ARDUINO_PORT_TTYUSB),$(SKETCH))

.PHONY: deploy/nano-old
deploy/nano-old:
	$(call deploy-arduino,arduino:avr:nano:cpu=atmega328old,$(DEPLOY_ARDUINO_PORT_TTYUSB),$(SKETCH))

.PHONY: deploy/attiny13a
deploy/attiny13a:
	$(call deploy-arduinoasisp,MicroCore:avr:13:clock=1M2,$(DEPLOY_ARDUINO_PORT_TTYUSB))

.PHONY: deploy/attiny45
deploy/attiny45:
	$(call deploy-arduinoasisp,ATTinyCore:avr:attinyx5:chip=45,$(DEPLOY_ARDUINO_PORT_TTYUSB))

.PHONY: deploy/attiny85
deploy/attiny85:
	$(call deploy-arduinoasisp,ATTinyCore:avr:attinyx5:chip=85,$(DEPLOY_ARDUINO_PORT_TTYUSB))
