# MemRet:
#   This microfunction will perform any calculations to determine an EA for 
#   return
# Args:
#   uCnt: The OPVal to calculate
#   uR1: The OPType to calculate
# Uses:
#   uTmp, uCnt
# Returns:
#   uR0: The calculated EA (passed value)
#
# MemRet:
    # Check that this is a memory Op (if so, eval its EA)
    0x01 if nyb uR1 0x40 EvalOp
    0x02 if nyb uR1 0x50 EvalOp
    0x03 if nyb uR1 0x60 EvalOp
    0x04 if nyb uR1 0x70 EvalOp
    0x05 if nyb uR1 0x80 EvalOp
    0x06 none; mov uPC uRet
    
