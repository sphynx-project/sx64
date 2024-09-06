#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <out.img>"
    exit 1
fi

output_file="$1"

dd if=/dev/zero bs=4096 count=1 of="$output_file" status=none

# echo -ne '\x01' | dd of="$output_file" bs=1 seek=4095 conv=notrunc status=none

echo "$output_file ok."
