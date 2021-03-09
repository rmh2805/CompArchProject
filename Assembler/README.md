# CompArch Assembler

A basic assembler framework designed to generate test programs for csci-453

## Usage
Usage is ugly folks. 

1. Build the tokenizer by running `make`. 
2. Run the tokenizer. It accepts assembly from `stdin` and outputs tokens to `stdout`
3. Run the parser (e.g. `minVAX.py`) with this token stream from `stdin`, and it will print the object file to `stdout`

An example command would be:
```
make
cat sample.asm | ./tokenizer | python3 minVAX.py > sample.obj
```

## Input File Format

### Segments
Assembler recognizes 2 segments (code and data), which can be switched between with dot 
directives (`.code` and `.data`).

In general, code execution will begin at memory address 0x00, and the data segment will begin at 
the first available 4-alligned address after the final assembled instruction.
    
### Labels
Labels can take the form `Label:` in either segment, and can also be written as an assignment 
(`Label = <value>`) in the data segment. They must begin with an alphabetical character (or an 
underscore) and then be composed entirely of alphanumerical characters and underscores. 

A declared label must be the first token on its line (whitespace is discarded). No label may be 
declared more than once, and non-assignment labels must be the only items on their line.

### Numerical Literals
The only supported numerical literals are unsigned integers. These may be decimal values, octal 
values (in form `0<value>`), or hexadecimal values (in form `0x<value>`).

### Data Initialization
Within the data segment, data values may be inserted in one-unit, two-unit, or four-unit blocks
(where units are the minimum addressable value), with the same alignment. Values declared 
without a directive or under the `.byte` directive will be one unit long, those declared under 
the `.half` and `.short` directives will be 2 units long, and those declared under the `.word`
directive will be 4 units long.

A directive will remain in effect until either the end of the current line or another directive 
is utilized. If no values are specified before either is encountered, a zero value of the proper 
size will be inserted.

As an example, the line `foo = 0x0f .half 2 .byte .word 010` under big endian encoding will 
become `0x0f ??? 0x00 0x02 0x00 ??? ??? ??? 0x00 0x00 0x00 0x08`, with label `foo` referencing 
the first unit (`0x0f`).

### Code Segment Structure
Up to one instruction per line. The instruction mnemonic should be the first token of the list, and 
should follow the same naming conventions as labels. After the mnemonic, arguments should be placed
as a comma separated list (`<mnemonic> [<operand 1>[, <operand 2>[, ...]]`).

### Operand Types/Addressing modes
**NOTE:** as used here, `<val>` may be any label or integer literal, and `<reg>` refers to a 
register formatted as in the register mode.

**NOTE ALSO:** Not all full implementations need encode all modes, but the framework will parse 
them.

**NOTE THE THIRD:** Yes, I did just copy the in-class notes on the VAX's supported addressing modes 
for this.

- Register (`REG`): `%<val>`
    - The value in the specified register
- Immediate (`IMM`): `$<val>`
    - The specified immediate value
- Absolute (`ABS`): `<val>`
    - `EA = <val>`
- Displacement (`DSP`): `<val>(<reg>)`
    - `EA = <val> + data(<reg>)`
- Register Indirect (`IDR`): `(<reg>)`
    - `EA = data(<reg>)`
- Memory Indirect (`IDM`): `@(<val>)`
    - `EA = mem[<val>]`
- Index (`IDX`): `(<reg> + <reg>)`
    - `EA = sum(data(<reg>))`
- Increment (`INC`): `(<reg>)+`
    - `EA = data(<reg>); <reg> <- data(<reg>) + 1`
- Decrement (`DEC`): `-(<reg>)`
    - `<reg> <- data(<reg>) - 1; EA = data(<reg>)`
- Scale (`SCL`): `<Other Arg>[<reg>]`
    - `EA = EA(<Other Arg>) + data(<reg>) * d`, for externally determined `d`
