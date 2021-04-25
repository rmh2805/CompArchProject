# Writeback:
    0x00 if noOps Fetch

    0x01 if byte IR 0x02 WritebackAdd       # Writeback for CLR 

    0x02 if byte IR 0x10 WritebackAdd       # Writeback for ADD
    0x03 if byte IR 0x11 WritebackAdd       # Writeback for SUB
    0x04 if byte IR 0x12 WritebackMul       # Writeback for MUL
    0x05 if byte IR 0x13 WritebackMul       # Writeback for DIV
    0x06 if byte IR 0x14 WritebackAdd       # Writeback for MOD
    0x07 if byte IR 0x15 WritebackAdd       # Writeback for AND
    0x08 if byte IR 0x16 WritebackAdd       # Writeback for OR
    0x09 if byte IR 0x17 WritebackAdd       # Writeback for XOR
    0x0A if byte IR 0x18 WritebackAdd       # Writeback for CMP
    0x0B if byte IR 0x19 WritebackInc       # Writeback for INC
    0x0C if byte IR 0x1A WritebackInc       # Writeback for DEC

    0x0D if byte IR 0x20 WritebackAdd       # Writeback for MOV
    0x0E if byte IR 0x21 WritebackSwp       # Writeback for SWP

    0x0F none; goto Fetch                   # Fallback catch

# WritebackAdd:
    # Apply prefixes (and truncation) to this value
    0x10 mov uR1 OP1Type; none
    0x11 mov uRet uPC goto PrefixRet
    0x12 mov OP4Val uR0; none               # Stash this value for latter

    # Calculate Scaled EA in case of memory op 1
    0x13 mov uCnt OP1Val; mov uR1 OP1Type
    0x14 mov uRet uPC; goto MemRet
    0x15 mov uCnt OP1Scale; mov uR1 uR0
    0x16 mov uRet uPC; goto ScaleOp

    # Write out with known values
    0x17 mov uR1 uR0; mov uR0 OP1Type
    0x18 mov uCnt OP1Val; mov uR2 OP4Val
    0x19 mov uRet uPC; goto WriteRet
    0x1A none; goto Fetch

# WritebackMul:
    0x1B mov OP3Val uR0; mov uR0 uR1    # Store uR0 for later, work on uR1

    
    # Apply prefixes (and truncation) to second return
    0x1C mov uR1 OP2Type; none
    0x1D mov uRet uPC goto PrefixRet
    0x1E mov OP4Val uR0; none               # Stash this value for latter

    # Calculate Scaled EA in case of memory OP 2
    0x1F mov uCnt OP2Val; mov uR1 OP2Type
    0x20 mov uRet uPC; goto MemRet
    0x21 mov uCnt OP2Scale; mov uR1 uR0
    0x22 mov uRet uPC; goto ScaleOp

    # Write out with known values
    0x23 mov uR1 uR0; mov uR0 OP2Type
    0x24 mov uCnt OP2Val; mov uR2 OP4Val
    0x25 mov uRet uPC; goto WriteRet
    0x26 none; goto WritebackAdd

# WritebackInc:
    # Restore original operand values and writeback as in ADD
    0x27 mov OP1Val OP2Val; mov OP1Type OP2Type
    0x28 mov OP1Scale OP2Scale; goto WritebackAdd

# WritebackSwp:
    # OP 1 <- OP 3, OP 2 <- OP 4
    0x29 mov OP1Val OP3Val; mov OP2Val OP4Val
    0x2A mov OP1Type OP3Type; mov OP3Type OP4Type
    0x2B mov OP1Scale OP3Scale; mov OP3Scale OP4Scale
    0x2C none; goto WritebackMul
