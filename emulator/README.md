# sx64 Emulator

## Overview

The **sx64 Emulator** is an implementation of the sx64 CPU architecture, designed to provide a simple and efficient 64-bit CPU model. This emulator aims to support the development and testing of software for the sx64 architecture, facilitating exploration and experimentation with its instruction set and architecture.

## Features

- **64-bit CPU Architecture:** A minimalistic design emphasizing simplicity and efficiency.
- **Instruction Set:** Supports basic instructions with variable lengths.
- **Memory and Bus System:** Simulates memory devices and a bus system for device interactions.
- **Debugging:** Includes comprehensive logging for instruction fetching, decoding, and execution.

## Getting Started

### Prerequisites

- C++11 or later
- [spdlog](https://github.com/gabime/spdlog) for logging, installed by the build script if you are on supported OS.

### Building

1. Clone the repository and cd into the emulator code:

    ```bash
    git clone https://github.com/sphynxos/sx64.git
    cd sx64/emulator
    ```

2. Build the project using our script:

    ```bash
    ./build.sh
    ```

### Usage

To run the emulator with premade BIOS (sys-bootstrap) and boot img (krnl-bootstrap):

```bash
./sx64-generic-emu -b ../bios.img ../boot.img
```

The emulator will initialize and start executing instructions. You can view detailed logs of CPU operations and memory interactions. To see debug messages simply add the *-v* flag or *-vv* for extra debug messages.

## Instruction Set

- **NOP:** No operation. Opcode: `0x00`, Size: 1 byte.
- **HLT:** Halt execution. Opcode: `0x01`, Size: 1 byte.

## Architecture

Read [DESIGN.txt](https://github.com/sphynxos/sx64/tree/main/DESIGN.txt) for a in depth design over the architecture.

## Copyright and Licensing

**Copyright © Kevin Alavik (shittydev.com) <kevin@alavik.se> <kevin@shittydev.com>**

The sx64 CPU design and emulator is both licensed under the MIT License.

## Contact

For further inquiries or contributions, please contact:

- **Kevin Alavik**
  - Email: [kevin@alavik.se](mailto:kevin@alavik.se)(mailto:kevin@shittydev.com)
  - [shittydev.com](https://shittydev.com)

For additional details, visit:

- [Sphynx Projects / OS](https://github.com/sphynxos)
- [sphynx.shittydev.com](https://sphynx.shittydev.com)