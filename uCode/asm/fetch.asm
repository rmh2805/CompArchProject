# Fetch:
    0x00 none;           none
                                            // Increment PC
    0x01 if pcmax goto halt
    0x02 add PC PC i1;   none
                                            // Load instruction into IR
    0x03 mov mar pc;     none
    0x04 read;           none
    0x05 mov ir mdr;     none

# FetchOperands:
    0x06 if noops goto Decode               // Skip opernads if there are none
                                            // Grab the first operand
    0x07 mov uRet uPC;       goto GetOperand
    0x08 mov OP1Type uR0;    mov OP1val uR1
    0x09 mov OP1Scale ur2;   none
                                            // Break checks
    0x0A if eq op1type i0 InterpOperand     // Break on explicit end
    0x0B if oneOp Decode                    // Break on implicit end

    # Grab the second operand
    0x0C mov uRet uPC;       goto GetOperand
    0x0D mov OP2Type ur0;    mov OP2Val uR1
    0x0E mov OP2Scale uR2;   none
                                            // Break checks
    0x0F if eq op2type i0 InterpOperand     // Break on explicit end
    0x10 if twoOps Decode                   // Break on implicit end

    # Grab the third operand
    0x11 mov uRet uPC;        goto GetOperand
    0x12 mov OP3Type ur0;     mov OP3Val uR1
    0x13 mov OP3Scale uR2;    none
                                            // Break checks
    0x14 if eq op3type i0 InterpOperand     // Break on explicit end
    0x15 if threeOps Decode                 // Break on implicit end

    # Grab the fourth operand
    0x16 mov uRet uPC;        goto GetOperand
    0x17 mov OP4Type ur0;     mov OP4Val uR1
    0x18 mov OP4Scale uR2;    none
                                            // Break checks
    0x19 if eq op4type i0  InterpOperand    // Break on explicit end
    0x1A none;               goto Decode    // Break on implicit end

# InterpOperand:
    # Control instructions
    0x1B if byte ir 0x04 InterpDmp          // Interpret DMP operands

    # ALU instructions
    0x1C if byte ir 0x10 InterpAdd          // Interpret ADD operands
    0x1D if byte ir 0x11 InterpAdd          // Interpret SUB operands
    0x1E if byte ir 0x15 InterpAdd          // Interpret AND operands
    0x1F if byte ir 0x16 InterpAdd          // Interpret OR operands
    0x20 if byte ir 0x17 InterpAdd          // Interpret XOR operands

    0x21 if byte ir 0x12 InterpMul          // Interpret MUL operands
    0x22 if byte ir 0x13 InterpDiv          // Interpret DIV operands
    0x23 if byte ir 0x18 InterpCmp          // Interpret CMP operands
    
    # Memory Instructions
    0x24 if byte ir 0x22 InterpCpy          // Interpret CPY operands

    # COL operands
    0x25 if byte ir 0x34 interpBeq          // Interpret BEQ operands
    0x26 if byte ir 0x35 interpBeq          // Interpret BNE operands
    0x27 if byte ir 0x36 interpBeq          // Interpret BGT operands
    0x28 if byte ir 0x37 interpBeq          // Interpret BGE operands
    0x29 if byte ir 0x38 interpBeq          // Interpret BIE operands
    0x2A if byte ir 0x39 interpBeq          // Interpret BDE operands

    # Failure
    0x2B none;              goto halt       // Unable to interpret these ops

# InterpDmp:
    0x2C if eq OP1Type i0 halt              // Cannot interp DMP from 0 ops
    0x2D mov OP2Val i0;     mov OP2Type i8  // OP2Val <- 0, OP2Type <- 0x10
    0x2E none;              goto Decode

# InterpAdd:
    0x2F if eq OP1Type i0 halt
    0x30 if eq OP2Type i0 halt
    
    # Op 3 <- Op 2, Op 2 <- Op 1
    0x31 mov OP3Type OP2Type; mov OP2Type OP1Type
    0x32 mov OP3Val OP2Val; mov OP2Val OP1Val
    0x33 mov OP3Scale OP2Scale; mov OP2Scale OP1Scale

    0x34 none;                      goto Decode

# InterpMul:
    0x35 if eq OP1Type i0 halt
    0x36 if eq OP2Type i0 halt
    0x37 if nEq OP3Type i0 InterpMul1

    # Interp MUL from 2 ops
    # OP 3 <- OP 1, OP 4 <- OP 2
    0x38 mov OP3Type OP1Type; mov OP4Type OP2Type
    0x39 mov OP3Val OP1Val; mov OP4Val OP2Val
    0x3A mov OP3Scale OP1Scale; mov OP4Scale OP2Scale

    0x3B none; goto InterpMul2
    
# InterpMul1:
    # Interpret MUL from 3 Ops
    # OP 3 <- OP 2, OP 4 <- OP 3
    0x3C mov OP3Type OP2Type; mov OP4Type OP3Type
    0x3D mov OP3Val OP2Val; mov OP4Val OP3Val
    0x3E mov OP3Scale OP2Scale; mov OP4Scale OP3Scale

# InterpMul2:
    # OP2 <- 0
    0x3F mov OP2Val i0; mov OP2Type i8
    0x40 mov OP2Scale i0; goto Decode

# InterpDiv:
    0x41 if eq OP1Type i0 halt
    0x42 if eq OP2Type i0 halt
    0x43 if nEq OP3Type i0 InterpDiv1

    # Interp DIV from 2 ops
    # OP 3 <- OP 1, OP 4 <- OP 2
    0x44 mov OP3Val OP1Val; mov OP4Val OP2Val
    0x45 mov OP3Type OP1Type; mov OP4Type OP2Type
    0x46 mov OP3Scale OP1Scale; mov OP4Scale OP2Scale

    0x47 none; goto Decode

# InterpDiv1:
    # Interpret DIV from 3 ops
    # OP 3 <- OP 2, OP 4 <- OP 3
    0x48 mov OP3Val OP2Val; mov OP4Val OP3Val
    0x49 mov OP3Type OP2Type; mov OP4Type OP3Type
    0x4A mov OP3Scale OP2Scale; mov OP4Scale OP3Scale

    # If Op 2 is an immediate, Op 2 <- Op 4
    0x4B if nyb OP2Type 0x10 InterpDiv2
    0x4C if nNyb OP2Type 0x20 Decode

# InterpDiv2:
    0x4D mov OP2Val OP4Val; mov OP2Type OP4Type
    0x4E mov OP2Scale OP4Scale; goto Decode

# InterpCmp:
    0x4F if eq OP1Type i0 halt

    # OP 2 <- OP 1
    0x50 mov OP2Type OP1Type; mov OP2Val OP1Val
    0x51 mov OP2Scale OP1Scale; goto Decode

# InterpCpy:
    0x52 if eq OP2Type i0 halt
    0x53 if eq OP2Type i0 halt
    0x54 if nEq OP3Type i0 InterpCpy1

    0x55 mov OP3Type i8; none
# InterpCpy1:
    0x56 mov OP4Type i8; goto Decode

# InterpBeq:
    0x57 if eq OP1Type i0 halt
    
    # OP 3 <- OP 2
    0x58 mov OP3Type OP2Type; mov OP3Val OP2Val
    0x59 mov OP3Scale OP2Scale; none

    # OP 2 <- 0
    0x5A mov OP3Type i8; goto Decode





