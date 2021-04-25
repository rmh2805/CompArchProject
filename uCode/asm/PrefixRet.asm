# PrefixRet:
#   This microfunction will apply prefix bytes to a return value
# Args:
#   uR0: The value to handle
#   uR1: The related OpType
# Uses:
#   uTmp
# Returns:
#   uR0: The return with prefixes applied (and any truncation)
#
# PrefixRet:
    # This is the same as PrefixOp unless this is a truncated Register    
    0x00 if nNyb uR1 0x30, goto PrefixOp
    0x01 and uTmp uR1 i7; none

    0x02 if nByte uTmp 0x00 +1
    0x03 and uR0 uR0 i7; goto PrefixOp
    0x04 if nByte uTmp 0x01 +1
    0x05 and uR0 uR0 i4; goto PrefixOp
    0x06 if nByte uTmp 0x02 +1
    0x07 and uR0 uR0 i5; goto PrefixOp
    0x08 none; goto PrefixOp
