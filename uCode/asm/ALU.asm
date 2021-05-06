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
# MOD:
    0x08 mov OP4Val OP3Val; mov OP3Val OP2Val
    0x09 mov uRet uPC; goto DO_DIV
    0x0A mov uR0 uR1; goto Writeback
# DIV:
    0x0B mov uRet uPC; goto DO_DIV
    0x0C none; goto Writeback

# DO_DIV:
    0x10 mov uR0 i0; mov uR2 i0         # Set output and sign flag to 0

    0x11 srl uTmp OP3Val if; none;      # Check the sign bit of the first op
    0x12 if nBits uTmp 0x80 +3
    0x13 cmp OP3Val OP3Val; none        # Invert the first opeprand
    0x14 add OP3Val OP3Val i1; none
    0x15 add uR2 uR2 i1; none           # Increment the negative counter

    0x16 srl uTmp OP4Val if; none;      # Check the sign bit of the second op
    0x17 if nBits uTmp 0x80 +3
    0x18 cmp OP4Val OP4Val; none        # Invert the second opeprand
    0x19 add OP4Val OP4Val i1; none
    0x1A add uR2 uR2 i1; none           # Increment the negative counter

# DO_DIV_LOOP:
    0x1B sub OP3Val OP3Val OP4Val; none # Remainder <- Remainder - dividend

    # Break on remainder sign bit set, else increment qotient and repeat loop
    0x1C srl uTmp OP3Val if; none;      
    0x1D if bits uTmp 0x80 goto DO_DIV_BREAK 
    0x1E add uR0 uR0 i1; goto DO_DIV_LOOP

# DO_DIV_BREAK:
    0x1F add uR1 OP3Val OP4Val; none # Set mod return value
    0x20 if nByte uR2 0x01 uRet
    0x21 cmp uR0 uR0; none;
    0x22 add uR0 uR0 i1; mov uPC uRet




# MUL:
    0x30 mov uCnt i3; mov uR0 i0
# MUL_LOOP:
    0x31 sll uR0 uR0 i1; none       # Double the current partial sum
    0x32 if nBits OP3Val 1 +1        # If bit not set, skip the next instruction
    0x33 add uR0 uR0 OP4Val; none   # Add second factor to partial sum
    
    # Break loop or update and restart loop
    0x34 if eq uCnt i0 Writeback
    0x35 srl OP3Val OP3Val i1; sub uCnt uCnt i1 
    0x36 none; goto MUL_LOOP
