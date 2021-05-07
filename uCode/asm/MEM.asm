# Microcode for all memory operations

# MOV:
    0x00 mov uR0 OP2Val; goto Writeback
# SWP:
    0x01 mov uR0 OP2Val; mov uR1 OP1Val
    0x02 none; goto Writeback
    
# CPY:
    # If ignoring OP4 limit, allow for intMax transfers
    0x10 if nEq OP4Val i0 +1 
    0x11 sub OP4Val i0 i1; none
    
    # Decode type
    0x12 if eq OP3Val i0 CPY_C
    0x13 if eq OP3Val i1 CPY_P
    0x14 mov uCnt OP3Val; goto CPY_P_1

# CPY_C:
    # Copy from src to dst, incrementing src along the way
    0x15 mov MAR OP1Val; add OP1Val OP1Val i1
    0x16 read; mov MAR OP2Val
    0x17 write; none
    
    # Decrement max count
    0x18 sub OP4Val OP4Val i1; none
    
    # Do break comparisons (end of string and OP 4) or inc dst and loop
    0x19 if eq MDR i0 Writeback
    0x1A if eq OP4Val i0 Writeback
    0x1B add OP2Val OP2Val i1; goto CPY_C
    
# CPY_P:
    # Copy the length byte across (and save)
    0x1C mov MAR OP1Val; add OP1Val OP1Val i1
    0x1D read; mov MAR OP2Val
    0x1E write; mov uCnt MDR
    
    # Decrement max count and check for completion
    0x1F sub OP4Val OP4Val i1; none
    0x20 if eq uCnt i0 Writeback
    0x21 if eq OP4Val i0 Writeback

# CPY_P_1:
    # Copy from src to dst, incrementing src along the way
    0x22 mov MAR OP1Val; add OP1Val OP1Val i1
    0x23 read; mov MAR OP2Val
    0x24 write; none
    
    # Decrement max count and lenCount
    0x25 sub uCnt uCnt i1; none
    0x26 sub OP4Val OP4Val i1; none
    
    # Do break comparisons (uCnt and OP 4) or inc dst and loop
    0x27 if eq uCnt i0 Writeback
    0x28 if eq OP4Val i0 Writeback
    0x29 add OP2Val OP2Val i1; goto CPY_P_1
