# LED Control Service

This repository contains a systemd service for controlling LEDs on Raspberry Pi 4B GPIO chips using libgpiod C++ bindings. The service is designed and coded based on the specifications in requirements.md.

This project was developed, written, and troubleshooted using Grok 4 by xAI, based on the requirements.md file.

## Installation
1. Clone the repo: `git clone https://github.com/KeithSBB/led-control-service.git`
2. Create build directory: `cd led-control-service; mkdir build; cd build`
3. Run CMake: `cmake ..`
4. Build: `make`
5. Install: `sudo make install`

This installs:
- Binaries: /usr/bin/led-service, /usr/bin/ledctl
- Config file: /etc/led-control/leds.ini (copy sample if needed)
- Systemd unit: /lib/systemd/system/led-control.service

Enable and start the service: `sudo systemctl enable --now led-control`

## Configuration
Create /etc/led-control/leds.ini with sections for each GPIO chip:

[gpiochip0]

red=23

green=24

blue=25

## Usage
Use `ledctl <command>` where <command> is the sequence string.

Examples:
- Turn on red permanently: `ledctl "red inf"`
- Turn on red and green permanently: `ledctl "(red inf, green inf)"`
- Turn off all: `ledctl "off"`
- Sequence: `ledctl "red 1.2, (fault 1.3, blue 2), yellow 3, OFF 4, loop forever"`
- Blink red: `ledctl "red 0.5, OFF 0.5, loop forever"`

Note: Chip prefix optional (default gpiochip0), ON optional.

## License
MIT
