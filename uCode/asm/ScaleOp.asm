# ScaleOp:
#   This microfunction will grab the full value of a given operand (memory 
#   access and prefix bytes)
# Args:
#   uCnt: The operand's OpScale
#   uR1: The operand's OpVal
# Uses:
#   uTmp, uR2
# Returns:
#   uR0: The value from OpVal with scaling applied (run EvalOp first for base)
#
# ScaleOp:
    0x00 if eq uCnt i0 uRet                 # Skip everything if not scaled
    
    0x01 srl uR2 uCnt iB; none  
    0x02 and uTmp uR2 i6; none              # Grab the rs value

    0x03 if nByte uTmp 0x00 +1              # Skip unless targeting r0
    0x04 mov uR0 r0; goto ScaleOp1          # Grab r0 and prepare to scale
    0x05 if nByte uTmp 0x04 +1              # Skip unless targeting r1
    0x06 mov uR0 r1; goto ScaleOp1          # Grab r1 and prepare to scale
    0x07 if nByte uTmp 0x08 +1              # Skip unless targeting pB
    0x08 mov uR0 pB; goto ScaleOp1          # Grab pB and prepare to scale

    0x09 mov uTmp uCnt; and uCnt uCnt i9    # Set uCnt for regVal
    0x0A mov uR2 uRet; none                 # Save uRet for restore
    0x0B mov uRet uPC; goto RegVal          # Grab the value of the register
    0x0C mov uCnt uTmp; none                # Restore uCnt

# ScaleOp1:
    0x0D srl uCnt uCnt iB; none             # discard reg byte
    0x0E and uCnt uCnt i7; none             # Extract val

    0x0F if nByte uCnt 0x00 +1              # Skip if idx not scaled by 1
    0x10 add uR0 uR0 uR1; mov uPC uRet      # Add reg value to base and return

    0x11 sll uTmp uR0 i1; mov uR2 uR0       # Store base and double of indexes
    0x12 add uR0 uR1 uTmp; none             # Add Set return to base + 2 * idx

    0x13 if byte uCnt 0x01 uRet             # If scaled by 2 return now

    0x14 if byte uCnt 0x03 +1               # Skip next if scaled by 4
    0x15 add uR0 uR0 uCnt; mov uPC uRet     # Add idx to return scale by 3
    0x16 add uR0 uR0 uTmp; mov uPC uRet     # Add 2 * idx to return scale by 4




