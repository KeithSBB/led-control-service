#!/bin/bash

# install.sh - Installation script for LED Control Service

# This script builds and installs the service on Fedora 42 for Raspberry Pi 4B.
# Run as regular user from the repository root. It will prompt for sudo when needed.

set -e  # Exit on error

# Check for required tools
command -v cmake >/dev/null 2>&1 || { echo "Error: cmake is required but not installed."; exit 1; }
command -v make >/dev/null 2>&1 || { echo "Error: make is required but not installed."; exit 1; }
command -v g++ >/dev/null 2>&1 || { echo "Error: g++ is required but not installed."; exit 1; }

# Clean build directory (use sudo to handle any permission-denied files from prior sudo builds)
if [ -d "build" ]; then
    sudo rm -rf build/*
else
    mkdir build
fi

cd build

# Run CMake
cmake ..

# Build
make

# Install (requires sudo)
sudo make install

# Ensure config directory exists
sudo mkdir -p /etc/led-control

# Reload systemd daemon
sudo systemctl daemon-reload

# Enable and start the service
sudo systemctl enable --now led-control.service

echo "Installation complete. The LED Control Service is now running."
echo "Configure LEDs in /etc/led-control/leds.ini and use 'ledctl' to control them."
