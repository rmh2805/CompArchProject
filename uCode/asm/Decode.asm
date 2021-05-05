# Decode:
    0x00 if noOps DecodeBranch
    
    0x01 if byte IR 0x02 DecodeBranch       # Decode for CLR (writeback only)
    0x02 if byte IR 0x03 DecodeDmp          # Decode for DMP
    0x03 if byte IR 0x04 DecodeOut          # Decode for OUT
    
    0x04 if byte IR 0x10 DecodeAdd          # Decode for ADD
    0x05 if byte IR 0x11 DecodeAdd          # Decode for SUB
    0x06 if byte IR 0x12 DecodeMUL          # Decode for MUL
    0x07 if byte IR 0x13 DecodeDIV          # Decode for DIV
    0x08 if byte IR 0x14 DecodeAdd          # Decode for MOD
    0x09 if byte IR 0x15 DecodeAdd          # Decode for AND
    0x0A if byte IR 0x16 DecodeAdd          # Decode for OR
    0x0B if byte IR 0x17 DecodeAdd          # Decode for XOR
    0x0C if byte IR 0x18 DecodeCmp          # Decode for CMP
    0x0D if byte IR 0x19 DecodeInc          # Decode for INC
    0x0E if byte IR 0x1A DecodeInc          # Decode for DEC

    0x0F if byte IR 0x20 DecodeCmp          # Decode for MOV
    0x10 if byte IR 0x21 DecodeSwp          # Decode for SWP
    0x11 if byte IR 0x22 DecodeCpy          # Decode for CPY

    0x12 if byte IR 0x31 DecodeJmp          # Decode for JMP
    0x13 if byte IR 0x32 DecodeJmp          # Decode for JAL
    0x14 if byte IR 0x34 DecodeBeq          # Decode for BEQ
    0x15 if byte IR 0x35 DecodeBeq          # Decode for BNE
    0x16 if byte IR 0x36 DecodeBeq          # Decode for BGT
    0x17 if byte IR 0x37 DecodeBeq          # Decode for BGE
    0x18 if byte IR 0x38 DecodeBie          # Decode for BIE
    0x19 if byte IR 0x39 DecodeBie          # Decode for BDE
    0x1A if byte IR 0x3A DecodeJmp          # Decode for BAO
    0x1B if byte IR 0x3B DecodeJmp          # Decode for BAC

# DecodeDmp:
    0x1C mov uCnt OP1Val; mov uR1 OP1Type   # Get Val/EA for OP 1
    0x1D mov uRet uPC; goto EvalOp
    0x1E mov uCnt OP1Scale; mov uR1 uR0     # Scale Op 1
    0x1F mov uRet uPC; goto ScaleOp
    0x20 mov uCnt uR0; mov uR1 OP1Type      # Get memory value of Op 1
    0x21 mov uRet uPC; goto MemOp
    0x22 mov uRet uPC; goto PrefixOp        # Apply prefixes to OP 1 and execute
    0x23 mov OP1Val uR0; goto DecodeBranch

# DecodeOut:
    0x24 mov uCnt OP2Val; mov uR1 OP2Type   # Get Val/EA for OP 2
    0x25 mov uRet uPC; goto EvalOp
    0x26 mov uCnt OP2Scale; mov uR1 uR0     # Scale Op 2
    0x27 mov uRet uPC; goto ScaleOp
    0x28 mov uCnt uR0; mov uR1 OP2Type      # Get memory value of Op 2
    0x29 mov uRet uPC; goto MemOp
    0x2A mov uRet uPC; goto PrefixOp        # Apply prefixes to OP 2
    0x2B mov OP2Val uR0; goto DecodeDmp

# DecodeAdd:
    # Halt on immediate dst
    0x2C if nyb OP1Type 0x10 Halt
    0x2D if nyb OP1Type 0x20 Halt

    0x2E mov uCnt OP2Val; mov uR1 OP2Type   # Get Val/EA for OP 2
    0x2F mov uRet uPC; goto EvalOp
    0x30 mov uCnt OP2Scale; mov uR1 uR0     # Scale Op 2
    0x31 mov uRet uPC; goto ScaleOp
    0x32 mov uCnt uR0; mov uR1 OP2Type      # Get memory value of Op 2
    0x33 mov uRet uPC; goto MemOp
    0x34 mov uRet uPC; goto PrefixOp        # Apply prefixes to OP 2
    0x35 mov OP2Val uR0; none

# DecodeAdd1:
    0x36 mov uCnt OP3Val; mov uR1 OP3Type   # Get Val/EA for OP 3
    0x37 mov uRet uPC; goto EvalOp
    0x38 mov uCnt OP3Scale; mov uR1 uR0     # Scale Op 3
    0x39 mov uRet uPC; goto ScaleOp
    0x3A mov uCnt uR0; mov uR1 OP3Type      # Get memory value of Op 3
    0x3B mov uRet uPC; goto MemOp
    0x3C mov uRet uPC; goto PrefixOp        # Apply prefixes to OP 1 and execute
    0x3D mov OP3Val uR0; goto DecodeBranch

# DecodeMul:
    # Halt on immediate dst
    0x3E if nyb OP1Type 0x10 Halt
    0x3F if nyb OP1Type 0x20 Halt

# DecodeMul1
    0x40 mov uCnt OP4Val; mov uR1 OP4Type   # Get Val/EA for OP 4
    0x41 mov uRet uPC; goto EvalOp
    0x42 mov uCnt OP4Scale; mov uR1 uR0     # Scale Op 4
    0x43 mov uRet uPC; goto ScaleOp
    0x44 mov uCnt uR0; mov uR1 OP4Type      # Get memory value of Op 4
    0x45 mov uRet uPC; goto MemOp
    0x46 mov uRet uPC; goto PrefixOp        # Apply prefixes to OP 4
    0x47 mov OP4Val uR0; goto DecodeAdd1

# DecodeDiv:
    # If dst is immediate, check if mod is
    0x48 if nyb OP1Type 0x10 DecodeDiv1 
    0x49 if nyb OP1Type 0x20 DecodeDiv1
    0x4A none; goto DecodeMul1

# DecodeDiv1:
    # Halt if mod is also immediate
    0x4B if nyb OP2Type 0x10 Halt
    0x4C if nyb OP2Type 0x20 Halt
    0x4D none; goto DecodeMul1

# DecodeCmp:
    0x4E if nyb OP1Type 0x10 Halt
    0x4F if nyb OP1Type 0x20 Halt

    0x50 mov uCnt OP2Val; mov uR1 OP2Type   # Get Val/EA for OP 2
    0x51 mov uRet uPC; goto EvalOp
    0x52 mov uCnt OP2Scale; mov uR1 uR0     # Scale Op 2
    0x53 mov uRet uPC; goto ScaleOp
    0x54 mov uCnt uR0; mov uR1 OP2Type      # Get memory value of Op 2
    0x55 mov uRet uPC; goto MemOp
    0x56 mov uRet uPC; goto PrefixOp        # Apply prefixes to OP 2
    0x57 mov OP2Val uR0; goto DecodeBranch

# DecodeInc:
    # Duplicate OP1 for writeback, then decode like cmp
    0x58 mov OP2Type OP1Type; mov OP2Val OP1Val
    0x59 mov OP2Scale OP1Scale; goto DecodeCmp

# DecodeSwp:
    0x5A if nyb OP2Type 0x10 Halt
    0x5B if nyb OP2Type 0x20 Halt

    # Save ops for writeback
    0x5C mov OP3Type OP1Type; mov OP4Type OP2Type
    0x5D mov OP3Val OP1Val; mov OP4Val OP1Val
    0x5E mov OP3Scale OP1Scale; mov OP4Scale OP1Scale

    # Decode OP 1 then branch to CMP
    0x5F mov uCnt OP1Val; mov uR1 OP1Type   # Get Val/EA for OP 1
    0x60 mov uRet uPC; goto EvalOp
    0x61 mov uCnt OP1Scale; mov uR1 uR0     # Scale Op 1
    0x62 mov uRet uPC; goto ScaleOp
    0x63 mov uCnt uR0; mov uR1 OP1Type      # Get memory value of Op 1
    0x64 mov uRet uPC; goto MemOp
    0x65 mov uRet uPC; goto PrefixOp        # Apply prefixes to OP 1
    0x66 mov OP1Val uR0; goto DecodeCmp

# DecodeCpy:
    0x67 if nyb OP1Type 0x40 DecodeCpy1     # Check theat OP 1 is a memory op
    0x68 if nyb OP1Type 0x50 DecodeCpy1
    0x69 if nyb OP1Type 0x60 DecodeCpy1
    0x6A if nyb OP1Type 0x70 DecodeCpy1
    0x6B if nyb OP1Type 0x80 DecodeCpy1
    0x6C none; goto halt                    # Halt (op1 must be memory)

# DecodeCpy1:
    0x6D if nyb OP2Type 0x40 DecodeCpy2     # Check theat OP 2 is a memory op
    0x6E if nyb OP2Type 0x50 DecodeCpy2
    0x6F if nyb OP2Type 0x60 DecodeCpy2
    0x70 if nyb OP2Type 0x70 DecodeCpy2
    0x71 if nyb OP2Type 0x80 DecodeCpy2
    0x72 none; goto halt                    # Halt (op2 must be memory)

# DecodeCpy2:
    0x73 mov uCnt OP1Val; mov uR1 OP1Type   # Get OP1's EA
    0x74 mov uRet uPC; goto EvalOp
    0x75 mov uCnt OP1Scale; mov uR1 uR0     # Scale OP 1
    0x76 mov uRet uPC; goto ScaleOp
    0x77 mov OP1Val uR0; none

    0x78 mov uCnt OP2Val; mov uR1 OP2Type   # Get OP2's EA
    0x79 mov uRet uPC; goto EvalOp
    0x7A mov uCnt OP2Scale; mov uR1 uR0     # Scale OP 2
    0x7B mov uRet uPC; goto ScaleOp
    0x7C mov OP2Val uR0; none

# DecodeJmp:
    0x7D if nyb OP1Type 0x40 DecodeJmp1     # Check theat OP 1 is a memory op
    0x7E if nyb OP1Type 0x50 DecodeJmp1
    0x7F if nyb OP1Type 0x60 DecodeJmp1
    0x80 if nyb OP1Type 0x70 DecodeJmp1
    0x81 if nyb OP1Type 0x80 DecodeJmp1
    0x82 none; goto halt                    # Halt (op1 must be memory)

# DecodeJmp1:
    0x83 mov uCnt OP1Val; mov uR1 OP1Type   # Get OP1's EA
    0x84 mov uRet uPC; goto EvalOp
    0x85 mov uCnt OP1Scale; mov uR1 uR0     # Scale OP 1
    0x86 mov uRet uPC; goto ScaleOp
    0x87 mov OP1Val uR0; goto DecodeBranch

# DecodeBeq:
    0x88 if nyb OP3Type 0x40 DecodeBeq1     # Check theat OP 1 is a memory op
    0x89 if nyb OP3Type 0x50 DecodeBeq1
    0x8A if nyb OP3Type 0x60 DecodeBeq1
    0x8B if nyb OP3Type 0x70 DecodeBeq1
    0x8C if nyb OP3Type 0x80 DecodeBeq1
    0x8D none; goto halt                    # Halt (op1 must be memory)

# DecodeBeq1:
    0x8E mov uCnt OP3Val; mov uR1 OP3Type   # Get OP 3's EA
    0x8F mov uRet uPC; goto EvalOp
    0x90 mov uCnt OP3Scale; mov uR1 uR0     # Scale OP 3
    0x91 mov uRet uPC; goto ScaleOp
    0x92 mov OP3Val uR0; goto DecodeOut

# DecodeBie:
    0x93 if nyb OP1Type 0x10 Halt
    0x94 if nyb OP1Type 0x20 Halt
    0x95 none; goto DecodeBeq

# Decode Branch
    # Control instructions
    0x96 if byte IR 0x00 HLT
    0x97 if byte IR 0x02 CLR
    0x98 if byte IR 0x03 DMP
    0x99 if byte IR 0x04 OUT

    # ALU instructions
    0x9A if byte IR 0x10 ADD
    0x9B if byte IR 0x11 SUB
    0x9C if byte IR 0x12 MUL
    0x9D if byte IR 0x13 DIV

    0x9E if byte IR 0x14 MOD
    0x9F if byte IR 0x15 AND
    0xA0 if byte IR 0x16 OR
    0xA1 if byte IR 0x17 XOR

    0xA2 if byte IR 0x18 CMP
    0xA3 if byte IR 0x19 INC
    0xA4 if byte IR 0x1A DEC

    # Memory instructions
    0xA5 if byte IR 0x20 MOV
    0xA6 if byte IR 0x21 SWP
    0xA7 if byte IR 0x22 CPY
    
    # COL instructions
    0xA8 if byte IR 0x31 JMP
    0xA9 if byte IR 0x32 JAL
    0xAA if byte IR 0x33 RET
    
    0xAB if byte IR 0x34 BEQ
    0xAC if byte IR 0x35 BNE
    0xAD if byte IR 0x36 BGT
    0xAE if byte IR 0x37 BGE

    0xAF if byte IR 0x38 BIE
    0xB0 if byte IR 0x39 BDE
    0xB1 if byte IR 0x3A BAO
    0xB2 if byte IR 0x3B BAC

    0xB3 none; goto NOP





