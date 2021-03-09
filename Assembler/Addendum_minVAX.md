# minVAX Adendum

Instruction mnemonics are the same as in the writeup. 

Registers 0 and 1 can be accessed as expected.

Supported operand types from the framework are register, immediate, absolute, and displacement. 

PC relative operands use the standard displacement format on register 2 (so, `<val>(%2)`).
