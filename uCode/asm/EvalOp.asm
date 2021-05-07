# EvalOp:
#   This microfunction will evaluate a provided operand (without any scaling, 
#   prefix handling, or EA dereferencing)
# Args:
#   uCnt: The operand's OpVal
#   uR1: The operand's OpType
# Uses:
#   uTmp, uR2
# Returns:
#   uR0: The computed EA (for memory operands) or value (for registers and 
#        immediates)
#
# EvalOp:
    0x00 if nyb uR1 0x10, goto EvalNyb      # Grab nybble imediate
    0x01 if nyb uR1 0x20, goto EvalImm      # Immediate already evaluated
    0x02 if nyb uR1 0x30, goto EvalReg      # Grab register value

    0x03 if nyb uR1 0x40, goto EvalReg      # Grab register value (as EA)
    0x04 if nyb uR1 0x50, goto EvalMem      # Grab memory value (as EA)
    0x05 if nyb uR1 0x60, goto EvalIdx      # Calculate index EA
    0x06 if nyb uR1 0x70, goto EvalDsp      # Calculate displacement EA

    0x07 if nyb uR1 0x80, goto EvalImm      # EA grabbed as immediate
    0x08 if nyb uR1 0x90, goto EvalReg      # Evaluate (post) increment arg
    0x09 if nyb uR1 0xA0, goto EvalDec      # Evaluate (pre) decrement arg
    0x0A if nyb uR1 0xC0, goto EvalInc      # Evaluate (pre)  increment arg
    0x0B if nyb uR1 0xD0, goto EvalPIc      # Evaluage (post) increment arg

    0x0C none; goto halt                    # Illegal instruction type

# EvalNyb:
    0x0D and uR0 uR1 iA; mov uPC uRet       # Grab nybble immediate and return

# EvalImm:
    0x0E mov uR0 uCnt; mov uPC uRet         # Grab passed immediate and return

# EvalReg:
    0x0F and uTmp uR1 i6; none              # Grab the rs value

    0x10 if nByte uTmp 0x00 +1              # Skip unless targeting r0
    0x11 mov uR0 r0; goto EvalReg1          # Grab r0 and truncate
    0x12 if nByte uTmp 0x04 +1              # Skip unless targeting r1
    0x13 mov uR0 r1; goto EvalReg1          # Grab r1 and truncate
    0x14 if nByte uTmp 0x08 +1              # Skip unless targeting pB
    0x15 mov uR0 pB; goto EvalReg1          # Grab pB and truncate

    0x16 mov uR2 uRet; none                 # Save uRet for restore
    0x17 mov uRet uPC; goto RegVal          # Grab the register's value

# EvalReg1:
    0x18 and uTmp uR1 i7; none              # Grab the rsz field

    0x19 if nByte uTmp 0x00 +1              # Skip unless targeting low byte
    0x1A and uR0 uR0 i9; mov uPC uRet       # Truncate and return
    0x1B if nByte uTmp 0x01 +1              # Skip unless targeting low half
    0x1C and uR0 uR0 i4; mov uPC uRet       # Truncate and return
    0x1D if nByte uTmp 0x02 +1              # Skip unless targeting low 3 bytes
    0x1E and uR0 uR0 i5; mov uPC uRet       # Truncate and return
    0x1F none; mov uPC uRet                 # return with full value

# EvalMem:
    0x20 mov uTmp i7; none                  # Set the loop counter
# EvalMemLoop:
    0x21 mov MAR uCnt; add uCnt uCnt i1     # Prepare to read the next byte
    0x22 read; srl uR0 uR0 iB               # Read it and make space to store
    0x23 sll uTmp2 MDR iF; none             # Shift it into the next position
    0x24 or uR0 uR0 uTmp2; none             # Incorporate it into return
    
    0x25 if eq uTmp i0 uRet                 # Either break loop or dec and loop
    0x26 sub uTmp uTmp i1; goto EvalMemLoop

# EvalIdx:
    0x2D and uTmp uR1 i6; none              # Grab the rs value

    0x2E if nByte uTmp 0x00 +1              # Skip unless targeting r0
    0x2F mov uR0 r0; goto EvalIdx1          # Grab r0 and grab second
    0x30 if nByte uTmp 0x04 +1              # Skip unless targeting r1
    0x31 mov uR1 r1; goto EvalIdx1          # Grab r1 and grab second
    0x32 if nByte uTmp 0x08 +1              # Skip unless targeting pB
    0x33 mov uR1 pB; goto EvalIdx1          # Grab pB and grab second

    0x34 mov uR2 uRet; none                 # Save uRet for restore
    0x35 mov uRet uPC; goto RegVal          # Grab the register's value

# EvalIdx1:
    0x36 and uTmp uR1 i6; mov uR1 uR0       # Grab the rs2 and store last val

    0x37 if nByte uTmp 0x00 +1              # Skip unless targeting r0
    0x38 mov uR0 r0; goto EvalIdx2          # Grab r0 and grab second
    0x39 if nByte uTmp 0x04 +1              # Skip unless targeting r1
    0x3A mov uR1 r1; goto EvalIdx2          # Grab r1 and grab second
    0x3B if nByte uTmp 0x08 +1              # Skip unless targeting pB
    0x3C mov uR1 pB; goto EvalIdx2          # Grab pB and grab second
    
    0x3D mov uR2 uRet; none                 # Save uRet for restore
    0x3E mov uRet uPC; goto RegVal          # Grab the register's value

# EvalIdx2:
    0x3F add uR0 uR0 uR1; mov uPC uRet      # Sum the values and return

# EvalDsp:
    0x40 and uTmp uR1 i6; none              # Grab the rs value

    0x41 if nByte uTmp 0x00 +1              # Skip unless targeting r0
    0x42 mov uR0 r0; goto EvalDsp1          # Grab r0 and go to summation
    0x43 if nByte uTmp 0x04 +1              # Skip unless targeting r1
    0x44 mov uR0 r1; goto EvalDsp1          # Grab r1 and go to summation
    0x45 if nByte uTmp 0x08 +1              # Skip unless targeting pB
    0x46 mov uR0 pB; goto EvalDsp1          # Grab pB and go to summation

    0x47 srl uCnt uR1 iF; mov uR1 uCnt      # uCnt <- reg specifier, uR1 <- imm
    0x48 mov uR2 uRet; none                 # Save uRet for retsore
    0x49 mov uRet uPC; goto RegVal          # Grab the register's value
    0x4A mov uCnt uR1; none                 # Return immediate to uCnt

# EvalDsp1:
    0x4B add uR0 uR0 uCnt; mov uPC uRet     # Calculate EA and return

# EvalInc:
    0x4C and uR2 uR1 i7; none
    0x4D add uR2 uR2 i1; goto EvalDec1

# EvalDec:
    0x4E mov uR2 i0; and uTmp uR1 i7
    0x4F add uTmp uTmp i1; none
    0x50 sub uR2 uR2 uTmp; none

# EvalDec1:
    0x51 and uTmp uR1 i6; none              # Grab the rs value
    0x52 or uR1 uR1 i7; none                # Premptively set truncation bits

    0x53 if nByte uTmp 0x00 +1              # Skip unless targeting r0
    0x54 and r0 r0 uR2; goto EvalReg        # Modify r0 and grab value
    0x55 if nByte uTmp 0x04 +1              # Skip unless targeting r1
    0x56 and r1 r1 uR2; goto EvalReg        # Modify r1 and grab value
    0x57 if nByte uTmp 0x08 +1              # Skip unless targeting pB
    0x58 and pB pB uR2; goto EvalReg        # Modify pB and grab value

    # Increment registers based on follow-on byte
    0x59 if nByte uCnt 0x00 +1
    0x5A add r0 r0 uR2; goto EvalReg
    0x5B if nByte uCnt 0x01 +1
    0x5C add r1 r1 uR2; goto EvalReg
    0x5D if nByte uCnt 0x02 +1
    0x5E add r2 r2 uR2; goto EvalReg
    0x5F if nByte uCnt 0x03 +1
    0x60 add r3 r3 uR2; goto EvalReg
    
    0x61 if nByte uCnt 0x04 +1
    0x62 add r4 r4 uR2; goto EvalReg
    0x63 if nByte uCnt 0x05 +1
    0x64 add r5 r5 uR2; goto EvalReg
    0x65 if nByte uCnt 0x06 +1
    0x66 add r6 r6 uR2; goto EvalReg
    0x67 if nByte uCnt 0x07 +1
    0x68 add r7 r7 uR2; goto EvalReg
    
    0x69 if nByte uCnt 0x08 +1
    0x6A add r8 r8 uR2; goto EvalReg
    0x6B if nByte uCnt 0x09 +1
    0x6C add r9 r9 uR2; goto EvalReg
    0x6D if nByte uCnt 0x0A +1
    0x6E add rA rA uR2; goto EvalReg
    0x6F if nByte uCnt 0x0B +1
    0x70 add rB rB uR2; goto EvalReg
    
    0x71 if nByte uCnt 0x0C +1
    0x72 add pR pR uR2; goto EvalReg
    0x73 if nByte uCnt 0x0D +1
    0x74 add pB pB uR2; goto EvalReg
    0x75 if nByte uCnt 0x0E +1
    0x76 add pS pS uR2; goto EvalReg
    0x77 if nByte uCnt 0x0F +1
    0x78 add PC PC uR2; goto EvalReg

    0x79 none; goto halt                    # Halt on bad register

# EvalPDc:
    0x7A mov uRet2 uRet; and uTmp2 uR1 i7   # Store context and get reg val
    0x7B or uR1 uR1 i7; none
    0x7C mov uRet uPC; goto EvalReg
    0x7D mov uRet uRet2; mov uTmp uTmp2
    
    0x7E mov uR2 i0; add uTmp uTmp i1       # Set dec val
    0x7F sub uR2 uR2 uTmp; goto EvalPIc1

# EvalPIc:
    0x80 mov uRet2 uRet; and uTmp2 uR1 i7   # Store context and get reg val
    0x81 or uR1 uR1 i7; none
    0x82 mov uRet uPC; goto EvalReg
    0x83 mov uRet uRet2; mov uR2 uTmp2

    0x84 add uR2 uR2 i1; none               # Set inc val

# EvalPIc1:
    0x85 and uTmp uR1 i6; none              # Grab the rs value

    0x86 if nByte uTmp 0x00 +1              # Skip unless targeting r0
    0x87 and r0 r0 uR2; mov uPC uRet        # Modify r0 and grab value
    0x88 if nByte uTmp 0x04 +1              # Skip unless targeting r1
    0x89 and r1 r1 uR2; mov uPC uRet        # Modify r1 and grab value
    0x8A if nByte uTmp 0x08 +1              # Skip unless targeting pB
    0x8B and pB pB uR2; mov uPC uRet        # Modify pB and grab value

    # Increment registers based on follow-on byte
    0x8C if nByte uCnt 0x00 +1
    0x8D add r0 r0 uR2; mov uPC uRet
    0x8E if nByte uCnt 0x01 +1
    0x8F add r1 r1 uR2; mov uPC uRet
    0x90 if nByte uCnt 0x02 +1
    0x91 add r2 r2 uR2; mov uPC uRet
    0x92 if nByte uCnt 0x03 +1
    0x93 add r3 r3 uR2; mov uPC uRet
    
    0x94 if nByte uCnt 0x04 +1
    0x95 add r4 r4 uR2; mov uPC uRet
    0x96 if nByte uCnt 0x05 +1
    0x97 add r5 r5 uR2; mov uPC uRet
    0x98 if nByte uCnt 0x06 +1
    0x99 add r6 r6 uR2; mov uPC uRet
    0x9A if nByte uCnt 0x07 +1
    0x9B add r7 r7 uR2; mov uPC uRet
    
    0x9C if nByte uCnt 0x08 +1
    0x9D add r8 r8 uR2; mov uPC uRet
    0x9E if nByte uCnt 0x09 +1
    0x9F add r9 r9 uR2; mov uPC uRet
    0xA0 if nByte uCnt 0x0A +1
    0xA1 add rA rA uR2; mov uPC uRet
    0xA2 if nByte uCnt 0x0B +1
    0xA3 add rB rB uR2; mov uPC uRet
    
    0xA4 if nByte uCnt 0x0C +1
    0xA5 add pR pR uR2; mov uPC uRet
    0xA6 if nByte uCnt 0x0D +1
    0xA7 add pB pB uR2; mov uPC uRet
    0xA8 if nByte uCnt 0x0E +1
    0xA9 add pS pS uR2; mov uPC uRet
    0xAA if nByte uCnt 0x0F +1
    0xAB add PC PC uR2; mov uPC uRet

    0xAC none; goto halt                    # Halt on bad register









