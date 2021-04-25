# WriteRet:
#   This microfunction will write out the provided value to the provided operand
# Args:
#   uCnt: The OPVal to target from
#   uR0: The OPType to target
#   uR1: A pre-calculated EA (because EvalOp is a nightmare)
#   uR2: The value to store (run through PrefixRet)
# Uses:
#   uTmp
# Returns:
#
# WriteRet:
    # Can safely return on immediates (accounted for in decode)
    0x00 if nyb uR0 0x10 uRet
    0x01 if nyb uR0 0x20 uRet

    # EA was precalculated, so we can just use that
    0x02 if nyb uR0 0x40 WriteRetMem
    0x03 if nyb uR0 0x50 WriteRetMem
    0x04 if nyb uR0 0x60 WriteRetMem
    0x05 if nyb uR0 0x70 WriteRetMem
    0x06 if nyb uR0 0x80 WriteRetMem

    # Write to all of the different register types (pre and post inc unified)
    0x07 if nyb uR0 0x30, goto WriteRetReg

    # Prep values for all increments and decrements
    0x08 and uTmp uR0 i7; none
    0x09 add uTmp uTmp i1; none

    # Perform increments and decrements during write
    0x0A if nyb uR0 0x90 WriteRetInc
    0x0B if nyb uR0 0xA0 WriteRetDec
    0x0C if nyb uR0 0xC0 WriteRetInc
    0x0D if nyb uR0 0xD0 WriteRetDec

    0x0E none; goto halt                    # Invalid operand type

# WriteRetMem:
    0x0F mov uCnt i7; none
# WriteRetMem1:
    0x10 mov MAR uR1; mov MDR uR2
    0x11 write; srl uR2 uR2 iB
    0x12 if byte uCnt 0x00 uRet
    0x13 add uR1 uR1 i1; none
    0x14 sub uCnt uCnt i1; goto WriteRetMem1

# WriteRetReg:
    0x15 and uTmp uR0 i6; none
    
    0x16 if nByte uTmp 0x00 +1
    0x17 mov r0 uR2; mov uPC uRet
    0x18 if nByte uTmp 0x04 +1
    0x19 mov r1 uR2; mov uPC uRet
    0x1A if nByte uTmp 0x08 +1
    0x1B mov pB uR2; mov uPC uRet

    0x1C and uTmp uCnt i9; none
    
    0x1D if nByte uTmp 0x00 +1
    0x1E mov r0 uR2; mov uPc uRet
    0x1F if nByte uTmp 0x01 +1
    0x20 mov r1 uR2; mov uPc uRet
    0x21 if nByte uTmp 0x02 +1
    0x22 mov r2 uR2; mov uPc uRet
    0x23 if nByte uTmp 0x03 +1
    0x24 mov r3 uR2; mov uPc uRet
    
    0x25 if nByte uTmp 0x04 +1
    0x26 mov r4 uR2; mov uPc uRet
    0x27 if nByte uTmp 0x05 +1
    0x28 mov r5 uR2; mov uPc uRet
    0x29 if nByte uTmp 0x06 +1
    0x2A mov r6 uR2; mov uPc uRet
    0x2B if nByte uTmp 0x07 +1
    0x2C mov r7 uR2; mov uPc uRet
    
    0x2D if nByte uTmp 0x08 +1
    0x2E mov r8 uR2; mov uPc uRet
    0x2F if nByte uTmp 0x09 +1
    0x30 mov r9 uR2; mov uPc uRet
    0x31 if nByte uTmp 0x0A +1
    0x32 mov rA uR2; mov uPc uRet
    0x33 if nByte uTmp 0x0B +1
    0x34 mov rB uR2; mov uPc uRet
    
    0x35 if nByte uTmp 0x0C +1
    0x36 mov pR uR2; mov uPc uRet
    0x37 if nByte uTmp 0x0D +1
    0x38 mov pB uR2; mov uPc uRet
    0x39 if nByte uTmp 0x0E +1
    0x3A mov pS uR2; mov uPc uRet
    0x3B if nByte uTmp 0x0F +1
    0x3C mov PC uR2; mov uPc uRet

    0x3D none; goto halt                    # Invalid register

# WriteRetInc:
    0x3E add uR2 uR2 uTmp; goto WriteRetReg
# WriteRetDec
    0x3F sub uR2 uR2 uTmp; goto WriteRetReg
