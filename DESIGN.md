# sx64 CPU Design

**Copyright © Kevin Alavik (shittydev.com) <kevin@alavik.se> <kevin@shittydev.com>**  
**The sx64 CPU design is licensed under the MIT License.**


## General Idea

The sx64 CPU architecture is a 64-bit design focusing on simplicity and efficiency. It aims to offer a clean and straightforward model that differs from traditional complex architectures like x86 or AArch64. The sx64 design emphasizes a minimalistic approach to support efficient and clear instruction processing.

## Registers

### General Purpose Registers

- **R0-R7**: Eight 64-bit registers used for various data storage tasks. They hold values for operations, function arguments, return values, counters, and pointers.

### Special Purpose Registers

- **SB (Stack Base)**: A 64-bit register defining the starting address of the stack. The stack grows upward in memory.
- **SP (Stack Pointer)**: A 64-bit register indicating the current top of the stack. It adjusts as data is added or removed.
- **IP (Instruction Pointer)**: A 64-bit register pointing to the address of the current instruction, advancing as the CPU executes instructions.
- **FR (Flags Register)**: A 16-bit register containing condition flags (Zero, Carry, Overflow, Sign) used for arithmetic and logical operations.

## Instruction Format

The instruction format for the sx64 CPU varies depending on the opcode. Each instruction consists of an opcode and, depending on the type of instruction, additional fields such as register indices, immediate values, or memory addresses.

### Common Instruction Fields

- **Opcode**: 1 byte (8 bits) defining the operation.
- **Register Index**: 1 byte (8 bits) specifying the register involved.
- **Immediate Value / Address**: 8 bytes (64 bits) for immediate values or memory addresses.

## Instruction List

### Opcodes and Formats

#### NOP (0x00)

- **Format**: `NOP`
- **Size**: 1 byte
- **Description**: No operation. The CPU does nothing for this instruction.
- **Example**: `NOP`

#### HLT (0x01)

- **Format**: `HLT`
- **Size**: 1 byte
- **Description**: Halt execution. Stops CPU execution.
- **Example**: `HLT`

#### WRITE (0x02)

- **Format**: `WRITE <Register> <Address>`
- **Size**: 1 byte (opcode) + 1 byte (register) + 8 bytes (address) = 10 bytes
- **Description**: Write the value of the specified register to the given memory address.
- **Example**: `WRITE R0, 0x0000000002001000`
  - **Opcode**: `0x02`
  - **Register**: `R0` (0x00)
  - **Address**: `0x0000000002001000` (8 bytes)

#### READ (0x03)

- **Format**: `READ <Register> <Address>`
- **Size**: 1 byte (opcode) + 1 byte (register) + 8 bytes (address) = 10 bytes
- **Description**: Read the value from the specified memory address into the given register.
- **Example**: `READ R1, 0x0000000002001000`
  - **Opcode**: `0x03`
  - **Register**: `R1` (0x01)
  - **Address**: `0x0000000002001000` (8 bytes)

#### LDI (0x04)

- **Format**: `LDI <Register> <ImmediateValue>`
- **Size**: 1 byte (opcode) + 1 byte (register) + 8 bytes (immediate value) = 10 bytes
- **Description**: Load an immediate value into the specified register.
- **Example**: `LDI R2, 0x0000000000000041` (or `LDI R2, 'A'`)
  - **Opcode**: `0x04`
  - **Register**: `R2` (0x02)
  - **Immediate Value**: `0x0000000000000041` (8 bytes)

## Performance and Timing

The sx64 CPU operates at a clock speed of 1 MHz. The execution time for each instruction is variable due to the differing lengths of instructions and their associated operands. The CPU maintains a timing mechanism to ensure consistent performance and adheres to a schedule to simulate real-time operation.

## Startup and Initialization

Upon startup, the sx64 CPU initializes by clearing all registers. It sets the Instruction Pointer (IP) to the address of the system bootstrap code (`sys-bootstrap`) to start hardware and CPU initialization. Following this, it jumps to the kernel bootstrap code (`krnl-bootstrap`) to load the operating system kernel. The exact addresses for these bootstraps are not predefined and are determined by the system configuration.

## Copyright Note

The sx64 CPU design is part of the Sphynx Projects / OS, a continuing development by Kevin Alavik. For additional details, visit [github.com/sphynxos](https://github.com/sphynxos) or [sphynx.shittydev.com](http://sphynx.shittydev.com). Contact Kevin Alavik at [kevin@alavik.se](mailto:kevin@alavik.se) or [kevin@shittydev.com](mailto:kevin@shittydev.com) for further inquiries.
