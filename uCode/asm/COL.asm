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


# RET HERE

# BEQ
    0x50 sub uTmp OP1Val OP2Val; none
    0x51 if nEq uTmp i0 Writeback
    0x52 mov PC OP3Val; goto Writeback



