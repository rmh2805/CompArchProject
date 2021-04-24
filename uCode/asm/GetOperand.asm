# GetOperand:
#   This microfunction will return the next operand from memory
# Returns:
#   uR0: Returns op type and prefix bytes. LSB is always the base operand type. 
#        For displacement operand, MSB is the register specifier byte (or 0). 2 
#        prefix bytes returned in displacement operands, 3 otherwise.
#
#   uR1: Returns the op value. Either immediate or register byte. For index 
#        operand, byte order is `<null> <null> <reg byte 1> <reg byte 2>`.
#
#   uR2: Returns any scale bytes (scale operand byte followed by scale reg byte)
#        Null if not a scaled operand.
#        

# GetOperand: 
    0x00 mov uR0 i0; mov uR1 i0
    0x01 mov uR2 i0; none

# GetOpType:
    0x02 if pcmax halt         # Increment PC
    0x03 add PC PC i1; none

    0x04 mov MAR PC; none           # Prepare to pull in the next op byte
    0x05 read; sll ur0 ur0 iB       # Pull next op byte, move back prefixes
    0x06 or uR0 uR0 MDR; none       # Insert next type byte into type buffer

    0x07 if nEq mdr i0 GetOpType2   # Skip if not operand terminator
    0x08 if nEq uR0 i0 halt         # Halt on Illegal operand (prefix bytes
                                    # on an opcode terminator)

    0x09 mov uPC uRet none          # Return since this is a legal 
                                    # terminator encoding

# GetOpType2:
    0x0A if nyb MDR 0x00 GetOpType
    0x0B none; goto FetchOpVal

# uRet:
    0x0C mov uPC uRet; none         # Micro return point

# FetchOpVal:
    # Break out to second stage fetch
    0x0D if nyb MDR 0x10 uRet           # Done (nybble immediate)
    
    0x0E if nyb MDR 0x20 FetchOpImm     # Fetch imm (for immediate)
    0x0F if nyb MDR 0x50 FetchOpImm     # Fetch imm (for mem indirect)
    0x10 if nyb MDR 0x80 FetchOpImm     # Fetch imm (for absolute address)
    
    0x11 if nyb MDR 0x30 FetchOpReg     # Fetch reg (for register)
    0x12 if nyb MDR 0x40 FetchOpReg     # Fetch reg (for indirect)
    0x13 if nyb MDR 0x90 FetchOpReg     # Fetch reg (for reg increment) 
    0x14 if nyb MDR 0xA0 FetchOpReg     # Fetch reg (for decrement)
    
    0x15 if nyb MDR 0x60 FetchOpIdx     # Fetch an index operand
    0x16 if nyb MDR 0x70 FetchOpDsp     # Fetch a displacement operand
    0x17 if nyb MDR 0xB0 FetchOpScl     # Fetch a scale operand
    
    0x18 none; goto halt                # Halt on Invalid address mode

# FetchOpImm:
    0x19 and uCnt MDR i7; none          # Set the immediate width register
# FetchOpImmLoop:
    0x1A if pcmax halt                  # Halt on PC overflow
    0x1B add PC PC i1; none             # Increment PC

    0x1C mov MAR PC; none               # Prepare to grab the next byte
    0x1D read; sll uR1 uR1 iB           # Grab next byte and prepare space
    0x1E or uR1 uR1 MDR; none           # incorporate IMM byte into return
    
    0x1F if eq uCnt i0 uRet             # Return on zero counter, else loop
    0x20 sub uCnt uCnt i1; goto FetchOpImmLoop

# FetchOpReg:
    # Skip grabbing the second byte if it's not specified
    0x21 if bits MDR 0x0C FetchOpReg2
    0x22 mov uPC uRet; none             # Return

# FetchOpReg2:
    0x23 if pcmax halt                  # Halt on PC overflow
    0x24 add PC PC i1; none             # Increment PC

    0x25 mov mar pc; none               # Target next byte
    0x26 read; sll uR1 uR1 iB           # Grab next byte and prepare space
    0x27 or uR1 uR1 MDR; none           # Incorporate reg byte into return

    0x28 mov uPC uRet; none             # Return

# FetchOpIdx:
    # Skip Grabbing the first register byte if not specified
    0x29 if nBits MDR 0x0C FetchOpIdx2
    0x2A mov uPC uRet; none             # Return

    0x2B if pcmax halt                  # Halt on PC overflow
    0x2C add PC PC i1; none             # Increment PC

    0x2D mov mar pc; none               # Target next byte
    0x2E read; sll uR1 uR1 iB           # Grab next byte and prepare space
    0x2F or uR1 uR1 MDR; none           # Incorporate reg byte into return

# FetchOpIdx2:
    0x30 none; sll uR1 uR1 iB           # Move up read byte
    0x31 if nBits uR0 0x03 uRet         # Possibly skip mem read
    
    0x32 if pcmax halt                  # Halt on PC overflow
    0x33 add PC PC i1; none             # Increment PC

    0x34 mov MAR PC; none               # Target Next Byte
    0x35 read; none                     # Grab the next byte
    0x36 or uR1 uR1 MDR; none           # Incorporate byte into return

    0x37 mov uPC uRet; none             # Return

# FetchOpDsp:
    0x38 and uR0 uR0 i5; mov uCnt i0    # Clear uR0 MSB and count register

    # Skip grabbing Reg byte if not specified
    0x39 if bits MDR 0x0C FetchOpImm

    0x3A if pcmax Halt                  # Halt on PC overflow
    0x3B add PC PC i1; none             # Increment PC

    0x3C mov MAR PC; none               # Target the next byte
    0x3D read; none                     # Read in the next byte
    0x3E mov uCnt MDR; none             # Store the register byte in a temp reg
    
    0x3F sll uCnt uCnt iF; none         # Shift the register byte up three bytes
    0x40 or uR0 uR0 uCnt; none          # Store the register byte in uR0

    0x41 none; goto FetchOpImm          # Grab the immediate for this mode

# FetchOpScl:
    0x42 mov uR2 MDR; and uR0 uR0 i2    # Move scale param to uR2 and clear it 
                                        # from uR0
    0x43 sll uR2 uR2 iB; none           # Make space in uR2 for the reg byte

    0x44 if bits MDR 0x03 FetchOpScl1

    0x45 if pcmax Halt                  # Halt on PC overflow
    0x46 add PC PC i1; none             # Increment the PC

    0x47 mov mar pc; none               # Target the next byte
    0x48 read; none                     # Read the register byte
    0x49 or uR2 uR2 MDR; none           # Store the register byte

# FetchOpScl1:
    0x4A if pcmax Halt                  # Halt on PC overflow
    0x4B add PC PC i1; none             # Increment the PC

    0x4C mov MAR PC; none               # Target the next byte
    0x4D read; none                     # Grab the next byte

    0x4E if nyb MDR 0x00 halt           # Fail on prefix byte
    0x4F if nyb MDR 0xB0 halt           # Fail on scale of scale

    # Store op type in reg and decode scaled op 
    0x50 or uR0 uR0 MDR; goto FetchOpVal
