# Prefix Op:
#   This microfunction will apply prefix bytes to a value
# Args:
#   uR0: The value to handle
#   uR1: The related OpType
# Uses:
#   uTmp
# Returns:
#   uR0: The value from OpVal with prefixes applied (and memory accessed) (run 
#        EvalOp and ScaleOp first to get value or EA)
#
# PrefixOp:
    0x00 if nNyb uR1 0x70 PrefixOp2         # Start loop if not dispacement
    0x01 and uR1 uR1 i5; goto PrefixOp2     # Yeet reg byte, start loop

# PrefixOp1:
    0x02 and uTmp uR1 i9; none              # Grab the next prefix byte
    
    0x03 if byte uTmp 0x00 uRet             # Break on null byte
    0x04 if nByte uTmp 0x01 +1              # if sign extension, extend sign
    0x05 ext uR0 uR0; goto PrefixOp2
    0x06 none; goto halt                    # halt on unrecognized prefix

# PrefixOp2:
    0x07 srl uR1 uR1 iB; goto PrefixOp1
