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

# MUL:
    0x10 mov uCnt i3; mov uR0 i0
# MUL_LOOP:
    0x11 sll uR0 uR0 i1; none       # Double the current partial sum
    0x12 if bits OP1Val 1 +1        # If bit not set, skip the next instruction
    0x13 add uR0 uR0 OP2Val; none   # Add second factor to partial sum
    
    # Break loop or update and restart loop
    0x14 if eq uCnt i0 Writeback
    0x15 srl OP1Val OP1Val i1; sub uCnt uCnt i1 
    0x16 none; goto MUL_LOOP
