# Micro Execute for each ALU Instruction

# ADD:
    0x00 add uR0 OP2Val OP3Val; goto Writeback
# SUB:
    0x01 sub uR0 OP2Val OP3Val; goto Writeback
# AND:
    0x02 and uR0 OP2Val OP3Val; goto Writeback
# OR:
    0x03 or uR0 OP2Val OP3Val; goto Writeback
# XOR:
    0x04 xor uR0 OP2Val OP3Val; goto Writeback
# CMP:
    0x05 cmp uR0 OP2Val; goto Writeback
# INC:
    0x06 add uR0 OP2Val i1; goto Writeback
# DEC:
    0x07 sub uR0 OP2Val i1; goto Writeback
