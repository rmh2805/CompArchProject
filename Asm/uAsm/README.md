# uAsm

An even more basic (read worse) assembler for our microcode format for the comp 
arch group project.

## Usage
### Basic process
1. Build the tokenizer by running `make`.
2. Run the tokenizer, which accepts micro assembly from `stdin` and outputs 
tokens to stdout
3. Feed this token stream into the assembler `assemble.py`'s `stdin`, and it 
will generate object code in `stdout`
4. Once all partial object files are generated, run them through the linker 
`link.py` to generate a unified object file

### Assembler
The assembler takes up to 2 arguments: 
1. A load point file specifying the absolute micro jump locations of all labels 
2. The file's load point, used to compute absolute jumps from relative jumps

The assembler was designed to run under python3.

### Linker
The linker takes as arguments:
1. An output file
2. A starting uPC value
3. A list of pairs of assembly files and their load points

The linker was designed to run under python 3.

### Micro assembly format
Our micro assembly format uses mnemonics as defined in `uCode/uCodeFmt.txt`, as
well as label formats as defined in the provided load point file

All micro instructions must start with their offset from the file load point 
(think Basic).

Conditional lines have `if` as their first non-offset word, followed by a 
condition mnemonic and any args it takes, and a destination.

Unconditional jumps are defined with a goto as the first word in their second 
parallel instruction.

All micro instructions must have instructions for both fields specified.

