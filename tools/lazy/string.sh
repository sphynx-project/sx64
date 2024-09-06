#!/bin/bash
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <string>"
    exit 1
fi

input_string="$1"

base_address=0x2001000

address=$base_address
for ((i=0; i<${#input_string}; i++)); do
    char="${input_string:$i:1}"
    hex_value=$(printf "%02X" "'$char")
    echo "ldi r0, 0x$hex_value"
    echo "write r0, 0x$(printf "%X" $address)"
done

echo "ldi r0, 0x0A"
echo "write r0, 0x$(printf "%X" $address)"