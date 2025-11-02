# LED Control Service

This repository contains a systemd service for controlling LEDs on Raspberry Pi 4B GPIO chips using libgpiod C++ bindings. The service is designed and coded based on the specifications in requirements.md.

This project was developed, written, and partially troubleshooted using Grok 4 by xAI, based on the requirements.md file.  Grok 4 failed to fix problems so 
a human performed the final fixes.

## Installation
1. Clone the repo: `git clone https://github.com/KeithSBB/led-control-service.git`
2. cd ~/led-control-service
3. run ./install.sh

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
Use `python client.py <hostname> <port> <command>` where `<command>` is the sequence string.

command examples:
- Turn on red permanently: `  "red inf"`
- Turn on red and green permanently: ` "(red inf, green inf)"`
- Turn off all: ` "off"`
- Sequence: ` "red 1.2, (fault 1.3, blue 2), yellow 3, OFF 4, loop forever"`
- Blink red: ` "red 0.5, OFF 0.5, loop forever"`
- Run sequebce five times: `  "red 1.5, green 1.5, blue 1.5, loop 5"`

Note: Chip prefix optional (default gpiochip0), ON optional.

## Home Assistant 
1. Install pyscript integration
2. place src/ledctl.py file into confi/pyscript directory
3. Useage as action (yaml):
   
  ```
action: pyscript.ledctl
data:
    hostname: barabashka.home.arpa
    port: "8080" 
    cmd: red 0.25, green 0.25, blue 0.25, loop forever
```


## License
MIT
