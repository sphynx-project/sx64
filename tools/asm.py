import sys
import struct
import re

# Opcode definitions
OPCODES = {
    "NOP": 0x00,
    "HLT": 0x01,
    "WRITE": 0x02,
    "READ": 0x03,
    "LDI": 0x04,
    "ADD": 0x05,
    "SUB": 0x06,
    "MUL": 0x07,
    "DIV": 0x08,
    "PUSH": 0x09,
    "POP": 0x0A,
    "JMP": 0x0B,
    "CMP": 0x0C,
    "JE": 0x0D,
    "JNE": 0x0E
}

# Register to index mapping
REGISTER_INDEX = {
    "R0": 0,
    "R1": 1,
    "R2": 2,
    "R3": 3,
    "R4": 4,
    "R5": 5,
    "R6": 6,
    "R7": 7
}

class Token:
    def __init__(self, type_, value):
        self.type = type_
        self.value = value

    def __repr__(self):
        return f"Token({self.type}, {self.value!r})"

class Lexer:
    def __init__(self, source):
        self.source = source
        self.position = 0
        self.current_char = self.source[self.position]

    def advance(self):
        self.position += 1
        if self.position > len(self.source) - 1:
            self.current_char = None
        else:
            self.current_char = self.source[self.position]
    
    def skip_whitespace(self):
        while self.current_char is not None and self.current_char.isspace():
            self.advance()

    def tokenize(self):
        tokens = []
        while self.current_char is not None:
            if self.current_char.isspace():
                if self.current_char == '\n':
                    tokens.append(Token('NEWLINE', '\n'))
                self.skip_whitespace()
                continue
            if self.current_char == ',':
                tokens.append(Token('COMMA', ','))
                self.advance()
                continue
            if self.current_char == ':':
                tokens.append(Token('COLON', ':'))
                self.advance()
                continue
            if self.current_char.isalpha():
                tokens.append(self.parse_keyword())
                continue
            if self.current_char in '0xX0oO0bB' or self.current_char.isdigit():
                tokens.append(self.parse_number())
                continue
            if self.current_char == "'":
                tokens.append(self.parse_char())
                continue
            raise ValueError(f"Unexpected character '{self.current_char}' at position {self.position}")
        return tokens

    def parse_keyword(self):
        value = ''
        while self.current_char is not None and (self.current_char.isalpha() or self.current_char.isdigit()):
            value += self.current_char
            self.advance()
        if re.match(r'^[Rr][0-7]$', value):
            return Token('REGISTER', value.upper())
        return Token('KEYWORD', value.upper())

    def parse_number(self):
        value = ''
        if self.current_char in '0xX':
            value += self.current_char
            self.advance()
            if self.current_char in 'xX':
                value += self.current_char
                self.advance()
            while self.current_char is not None and (self.current_char.isdigit() or self.current_char in 'ABCDEFabcdef'):
                value += self.current_char
                self.advance()
            return Token('NUMBER', int(value, 16))
        elif self.current_char in '0oO':
            value += self.current_char
            self.advance()
            if self.current_char in 'oO':
                value += self.current_char
                self.advance()
            while self.current_char is not None and (self.current_char.isdigit()):
                value += self.current_char
                self.advance()
            return Token('NUMBER', int(value, 8))
        elif self.current_char in '0bB':
            value += self.current_char
            self.advance()
            if self.current_char in 'bB':
                value += self.current_char
                self.advance()
            while self.current_char is not None and (self.current_char in '01'):
                value += self.current_char
                self.advance()
            return Token('NUMBER', int(value, 2))
        else:
            while self.current_char is not None and self.current_char.isdigit():
                value += self.current_char
                self.advance()
            return Token('NUMBER', int(value, 10))

    def parse_char(self):
        self.advance()  # Skip the opening quote
        if self.current_char is None or self.current_char == "'":
            raise ValueError(f"Empty character literal at position {self.position}")
        value = self.current_char
        self.advance()  # Skip the character
        if self.current_char != "'":
            raise ValueError(f"Character literal not properly closed at position {self.position}")
        self.advance()  # Skip the closing quote
        return Token('CHAR', ord(value))

class Assembler:
    def __init__(self, tokens):
        self.tokens = tokens
        self.index = 0
        self.address = 0

    def current_token(self):
        if self.index < len(self.tokens):
            return self.tokens[self.index]
        return None

    def eat(self, token_type):
        token = self.current_token()
        if token is not None and token.type == token_type:
            self.index += 1
            return token
        if token is None:
            raise ValueError(f"Expected token type {token_type} but got EOF")
        raise ValueError(f"Expected token type {token_type} but got {token.type} with value {token.value}")

    def parse(self):
        output_bytes = bytearray()
        while self.index < len(self.tokens):
            token = self.current_token()
            if token.type == 'KEYWORD':
                op = self.eat('KEYWORD').value
                if op not in OPCODES:
                    raise ValueError(f"Unknown instruction '{op}'")
                output_bytes.append(OPCODES[op])  # Write opcode

                if op in ["WRITE", "READ"]:
                    self.handle_register_operand(output_bytes)
                    self.handle_address_operand(output_bytes)

                elif op == "LDI":
                    self.handle_register_operand(output_bytes)
                    self.handle_immediate_operand(output_bytes)

                elif op in ["NOP", "HLT"]:
                    pass  # No additional operands

                elif op in ["ADD", "SUB", "MUL", "DIV"]:
                    self.handle_register_operand(output_bytes)
                    self.handle_register_operand(output_bytes)

                elif op == "PUSH" or op == "POP":
                    self.handle_register_operand(output_bytes)

                elif op in ["CMP", "JE", "JNE", "JMP"]:
                    self.handle_address_operand(output_bytes)

            elif token.type == 'NEWLINE':
                self.eat('NEWLINE')  # Skip newline characters

        return output_bytes

    def handle_register_operand(self, output_bytes):
        if self.current_token().type != 'REGISTER':
            raise ValueError(f"Expected register but got {self.current_token().type} with value {self.current_token().value}")
        reg = self.eat('REGISTER').value
        if reg not in REGISTER_INDEX:
            raise ValueError(f"Unknown register '{reg}'")
        output_bytes.append(REGISTER_INDEX[reg])

        if self.current_token().type == 'COMMA':
            self.eat('COMMA')  # Skip comma

    def handle_address_operand(self, output_bytes):
        address_token = self.current_token()
        if address_token.type not in ['NUMBER', 'CHAR']:
            raise ValueError(f"Expected number or char but got {address_token.type} with value {address_token.value}")
        self.eat(address_token.type)
        address = address_token.value
        self.write_address(output_bytes, address)

    def handle_immediate_operand(self, output_bytes):
        imm_token = self.current_token()
        if imm_token.type not in ['NUMBER', 'CHAR']:
            raise ValueError(f"Expected number or char but got {imm_token.type} with value {imm_token.value}")
        self.eat(imm_token.type)
        imm_value = imm_token.value
        self.write_address(output_bytes, imm_value)

    def write_address(self, output_bytes, address):
        output_bytes.extend(struct.pack('<Q', address))

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 assembler.py <source_file> <output_file>")
        sys.exit(1)

    source_file = sys.argv[1]
    output_file = sys.argv[2]

    try:
        with open(source_file, 'r') as f:
            source_code = f.read()

        lexer = Lexer(source_code)
        tokens = lexer.tokenize()

        assembler = Assembler(tokens)
        program_bytes = assembler.parse()

        with open(output_file, 'wb') as f:
            f.write(program_bytes)

        # Print the raw bytes to stdout
        sys.stdout.buffer.write(program_bytes)
        
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
