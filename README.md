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
    - The default value of this arg is `../uCode/asm/uCode.obj` relative to 
    the execution location

## Included Files
- src: Main source code directory
    - CPU.h: the main entrypoint for the simulation
    - cpu.h/cpu.cpp: define the actual simulation (hardware, linking, and 
    controls)
    - includes.h: General includes file, ensures that all files get everything 
    they need to function
    - testFiles/*: get their own section

- Asm: Assembler code
    - Assembler/*: All code for the macro-code assembler
    - uAsm/*: All code for the micro-code assembler and linker

- HW_Docs
    - The files already submitted earlier describing our hardware

- ISA_Docs: The documents comprising our ISA
    - ISA_doc.txt: Our compiled ISA document

- uCode: all information pertaining to our micro code
    - asm/*: The assembly and object files implementing the microcode
    - rtl/*: The RTL files originally used to describe the code
    - uCodeFmt.txt: Defines the microcode format


## Included Tests

## Additional Notes
- Although `OUT` was implemented, the simulated device attached never was, so 
its entire function is to write to a register that is never utilized.
