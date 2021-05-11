# Computer Architecture Project - worseVax
By Ray Healy and Ian Campbell

## Description 
Our project strove to create only the worst knockoff immitation of a subset of 
what VAX could do. More seriously, we chose to develop a microcoded CPU with 
complex instruction encodings, numerous addressing modes, the ability to accept 
variable numbers of operands for a given instruction, and *some* complex 
commands.

To this end we support 13 different instruction modes (including regular 
suspects like registers, immediates, and displacement, as well as stranger ones 
like memory indirect and scaled operands), and many instructions, most of which 
accept varying numbers of operands of most types, performing extra 
interpretation as needed to fill out their operand lists.

## Basic Usage
- After installing the repo, run `make` in directory `src` to generate the 
simulator
- Usage: `CPU <obj file> [microTrace on y/n] [alternate microcode file]`
  - The object file is the only requirement, and is the program to run
  - The 2nd arg will turn on micro trace output if its first character is 'y'
  - The 3rd arg specifes an alternative file to load microcode from
    - The default value of this arg is `uCode/asm/uCode.obj`

## Included Files

## Included Tests

## Additional Notes
