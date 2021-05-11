# minVAX Adendum

Instruction mnemonics are the same as in the writeup. 

Registers 0 and 1 can be accessed as expected.

Supported operand types from the framework are register, immediate, absolute, and displacement. 

PC relative operands use the standard displacement format on register 2 (so, `<val>(%2)`).

Added 4 additional register: delay1, delay2, ALU2Carry and ALU2OFlow. These registers facilitate storing ALU2's 
carry and overflow output. The delay registers are required to store the value every instruction. We then only push
them to the system level registers if we have a macro ADD/SUB instruction. 
