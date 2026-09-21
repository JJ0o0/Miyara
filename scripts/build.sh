#!/bin/bash

set -e

# Directories
BUILD_DIR="build"
ISO_DIR="$BUILD_DIR/iso"
DIST_DIR="dist"

# Terminal
RESET="\033[0m"
BOLD="\033[1m"
GREEN="\033[32m"
RED="\033[31m"
CYAN="\033[36m"

# Run a build step silently.
# If it fails, show its output and exit.
run_step()
{
    local message="$1"
    shift

    printf "\r\033[K${CYAN}%s${RESET}" "$message"

    local output
    if ! output=$("$@" 2>&1); then
        printf "\r\033[K${RED}✗ %s${RESET}\n" "$message"
        printf '%s\n' "$output"
        exit 1
    fi

    printf "\r\033[K${GREEN}✓ %s${RESET}\n" "$message"
}

# Creating distribution directory
mkdir -p "$DIST_DIR"

# Calling Make
run_step "[1/4] Calling make..." make

# ISO folder preparation
printf "\r\033[K${CYAN}[2/4] Preparing ISO directory...${RESET}"

rm -rf "$ISO_DIR"
mkdir -p "$ISO_DIR/boot/grub"

cp grub/grub.cfg "$ISO_DIR/boot/grub/grub.cfg"
cp "$BUILD_DIR/kernel.elf" "$ISO_DIR/boot/kernel.elf"

printf "\r\033[K${GREEN}✓ [2/4] Preparing ISO directory${RESET}\n"

# Creating ISO
run_step "[3/4] Creating ISO..." \
    grub-mkrescue -o "$DIST_DIR/miyara.iso" "$ISO_DIR"

# Finished
printf "\r\033[K${GREEN}${BOLD}✓ [4/4] Miyara build finished!${RESET}\n"
printf "    Output: ${BOLD}%s/miyara.iso${RESET}\n" "$DIST_DIR"