# Micro Execution RTL for COL

# JMP:
    0x00 mov PC OP1Val; goto Writeback

# JAL:
    0x10 mov uTmp pB; sub uR0 pS iD         # Prepare to write out pB
    0x11 mov uCnt i7; none                  # (Set count reg for write width)

# JAL_LOOP_PB:
    0x12 mov MAR uCnt; mov MDR uTmp         # Prepare write
    0x13 write; none                        # Write
    0x14 srl uTmp uTmp iB; add uR0 uR0 i1   # Shift output down and retarget
    0x15 if byte uCnt 0x00 JAL_BREAK_PB     # Break after final byte
    0x16 sub uCnt uCnt i1; goto JAL_LOOP_PB # Sub from counter and restart loop
# JAL_BREAK_PB:
    0x17 sub pB pS iD; none                 # Quickly retarget pB
    0x18 mov uTmp pR; sub uR0 pS iB         # Prepare to write out pR
    0x19 mov uCnt i7; none                  # (Set count reg for write width)

# JAL_LOOP_PR:
    0x1A mov MAR uCnt; mov MDR uTmp         # Prepare write
    0x1B write; none                        # Write
    0x1C srl uTmp uTmp iB; add uR0 uR0 i1   # Shift output down and retarget
    0x1D if byte uCnt 0x00 JAL_BREAK_PR     # Break after final byte
    0x1E sub uCnt uCnt i1; goto JAL_LOOP_PR # Sub from counter and restart loop
# JAL_BREAK_PR:
    0x1F sub pS pS iB; mov pR PC            # pS <- pS - 8, pR <- PC
    0x20 mov PC OP1Val; goto Writeback      # PC <- dst, return


# RET:
    0x30 mov PC pR; none                    # Use current return ptr

    0x31 sub uR0 pB iD; mov uCnt i7         # Prepare for pR read
# RET_LOOP_PR:
    0x32 mov MAR uR0; none
    0x33 read; sll pR pR iB                 # Read and shift for next byte
    0x34 or pR pR MDR; add uR0 uR0 i1       # Insert next byte
    0x35 if eq uCnt i0 RET_BREAK_PR
    0x36 sub uCnt uCnt i1; goto RET_LOOP_PR
# RET_BREAK_PR:
    0x37 add pS pB iD; none                 # Restore PS

    0x38 mov uCnt i8; mov uR0 pB            # Prepare for pB Read
# RET_LOOP_PB:
    0x39 mov MAR uR0; none
    0x3A read; sll pB pB iB                 # Read and shift for next byte
    0x3B or pB pB MDR; add uR0 uR0 i1       # Insert next byte
    0x3C if eq uCnt i0 Writeback
    0x3D sub uCnt uCnt i1; goto RET_LOOP_PB

# BEQ:
    0x40 sub uTmp OP1Val OP2Val; none
    0x41 if nEq uTmp i0 Writeback
    0x42 mov PC OP3Val; goto Writeback
# BNE:
    0x43 sub uTmp OP1Val OP2Val; none
    0x44 if eq uTmp i0 Writeback
    0x45 mov PC OP3Val; goto Writeback

# BGT
    0x50 sub uTmp OP2Val OP1Val; none       # uTmp <- OP1Val - OP2Val
    0x51 sll uTmp uTmp iF; none             # uTmp <- uTmp >> 24
    0x52 if nBits uTmp 0x80; Writeback      # No work if not sign bit
    0x53 mov PC OP3Val; goto Writeback      # Update PC if sign bit
# BGE
    0x54 sub uTmp OP1Val OP2Val; none       # uTmp <- OP2Val - OP1Val
    0x55 sll uTmp uTmp iF; none             # uTmp <- uTmp >> 24
    0x56 if bits uTmp 0x80; Writeback       # No work if sign bit
    0x57 mov PC OP3Val; goto Writeback      # Update PC if not sign bit

# BIE:
    0x60 add uTmp OP1Val i1; none           # Compute the basic increment
    0x61 mov uR0 uTmp; sub uTmp uTmp OP2Val # Save incremented value and compare
    0x62 if neq uTmp i0 Writeback
    0x63 mov PC OP3Val; goto Writeback
# BDE:
    0x64 sub uTmp OP1Val i1; none           # Compute the basic decrement
    0x65 mov uR0 uTmp; sub uTmp uTmp OP2Val # Save incremented value and compare
    0x66 if neq uTmp i0 Writeback
    0x67 mov PC OP3Val; goto Writeback



