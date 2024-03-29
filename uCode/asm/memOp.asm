# MemOp:
#   This microfunction will grab an operand from memory from its EA
# Args:
#   uCnt: The (pre-scaled) EA to grab
#   uR1: The related OpType
# Uses:
#   uTmp, uCnt, uR2
# Returns:
#   uR0: The value referenced by the operand, grabbed from mem if necessary
#
# MemOp:
    # If this is an EA we must grab it from memory
    0x00 if nyb uR1 0x40 MemOp1                 # Register Indirect
    0x01 if nyb uR1 0x50 MemOp1                 # Memory Indirect
    0x02 if nyb uR1 0x60 MemOp1                 # Index
    0x03 if nyb uR1 0x70 MemOp1                 # Displacement
    0x04 if nyb uR1 0x80 MemOp1                 # Absolute
    0x05 none; mov uPC uRet                     # Return on non-memory operand

# MemOp1:
    0x06 mov uTmp i7; none
# MemOp2:
    0x07 mov MAR uCnt; srl uR0 uR0 iB
    0x08 read; none
    0x09 sll uR2 MDR iF; none
    0x0A or uR0 uR0 uR2; none
    
    0x0B if eq uTmp i0 uRet
    0x0C add uCnt uCnt i1; none
    0x0D sub uTmp uTmp i1; goto MemOp2
