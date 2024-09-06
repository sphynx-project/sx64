#!/bin/bash
python3 ../tools/asm.py $(pwd)/../firmware/bios.asm ../bios.bin
python3 ../tools/asm.py $(pwd)/../firmware/boot.asm ../boot.bin