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
    0x00 and uR0 uCnt iA; none              # Mask off the low nybble

    0x01 if nByte uR0 0x00 +1
    0x02 mov uR0 r0; goto RegValRet
    0x03 if nByte uR0 0x01 +1
    0x04 mov uR0 r1; goto RegValRet
    0x05 if nByte uR0 0x02 +1
    0x06 mov uR0 r2; goto RegValRet
    0x07 if nByte uR0 0x03 +1
    0x08 mov uR0 r3; goto RegValRet
    
    0x09 if nByte uR0 0x04 +1
    0x0A mov uR0 r4; goto RegValRet
    0x0B if nByte uR0 0x05 +1
    0x0C mov uR0 r5; goto RegValRet
    0x0D if nByte uR0 0x06 +1
    0x0E mov uR0 r6; goto RegValRet
    0x0F if nByte uR0 0x07 +1
    0x10 mov uR0 r7; goto RegValRet
    
    0x11 if nByte uR0 0x08 +1
    0x12 mov uR0 r8; goto RegValRet
    0x13 if nByte uR0 0x09 +1
    0x14 mov uR0 r9; goto RegValRet
    0x15 if nByte uR0 0x0A +1
    0x16 mov uR0 rA; goto RegValRet
    0x17 if nByte uR0 0x0B +1
    0x18 mov uR0 rB; goto RegValRet
    
    0x19 if nByte uR0 0x0C +1
    0x1A mov uR0 pR; goto RegValRet
    0x1B if nByte uR0 0x0D +1
    0x1C mov uR0 pB; goto RegValRet
    0x1D if nByte uR0 0x0E +1
    0x1E mov uR0 pS; goto RegValRet
    0x1F if nByte uR0 0x0F +1
    0x20 mov uR0 PC; goto RegValRet

# RegValRet:
    0x21 mov uPC uRet; mov uRet uR2

