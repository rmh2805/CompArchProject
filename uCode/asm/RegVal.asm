# RegVal:
#   This microfunction will return the value stored within a (programmer 
#   visible) register and restore uRet to a passed value on its return
# Args:
#   uCnt: The index into the register file
#   uR2: The uRet of the caller (restored on exit)
# Uses:
#   No others
# Returns:
#   uR0: The value from the specified register
#
# RegVal:
    0x00 if nByte uCnt 0x00 +1
    0x01 mov uR0 r0; goto RegValRet
    0x02 if nByte uCnt 0x01 +1
    0x03 mov uR0 r1; goto RegValRet
    0x04 if nByte uCnt 0x02 +1
    0x05 mov uR0 r2; goto RegValRet
    0x06 if nByte uCnt 0x03 +1
    0x07 mov uR0 r3; goto RegValRet
    
    0x08 if nByte uCnt 0x04 +1
    0x09 mov uR0 r4; goto RegValRet
    0x0A if nByte uCnt 0x05 +1
    0x0B mov uR0 r5; goto RegValRet
    0x0C if nByte uCnt 0x06 +1
    0x0D mov uR0 r6; goto RegValRet
    0x0E if nByte uCnt 0x07 +1
    0x0F mov uR0 r7; goto RegValRet
    
    0x10 if nByte uCnt 0x08 +1
    0x11 mov uR0 r8; goto RegValRet
    0x12 if nByte uCnt 0x09 +1
    0x13 mov uR0 r9; goto RegValRet
    0x14 if nByte uCnt 0x0A +1
    0x15 mov uR0 rA; goto RegValRet
    0x16 if nByte uCnt 0x0B +1
    0x17 mov uR0 rB; goto RegValRet
    
    0x18 if nByte uCnt 0x0C +1
    0x19 mov uR0 pR; goto RegValRet
    0x1A if nByte uCnt 0x0D +1
    0x1B mov uR0 pB; goto RegValRet
    0x1C if nByte uCnt 0x0E +1
    0x1D mov uR0 pS; goto RegValRet
    0x1E if nByte uCnt 0x0F +1
    0x1F mov uR0 PC; goto RegValRet

# RegValRet:
    0x20 mov uPC uRet; mov uRet uR2

