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
    0x1D read; srl uR1 uR1 iB           # Grab next byte and prepare space
    0x1E sll uTmp MDR iF; none          # Shift it to the next position
    0x1F or uR1 uR1 uTmp; none          # incorporate IMM byte into return
    
    0x20 if eq uCnt i0 uRet             # Return on zero counter, else loop
    0x21 sub uCnt uCnt i1; goto FetchOpImmLoop

# FetchOpReg:
    # Skip grabbing the second byte if it's not specified
    0x22 if bits MDR 0x0C FetchOpReg2
    0x23 mov uPC uRet; none             # Return

# FetchOpReg2:
    0x24 if pcmax halt                  # Halt on PC overflow
    0x25 add PC PC i1; none             # Increment PC

    0x26 mov mar pc; none               # Target next byte
    0x27 read; sll uR1 uR1 iB           # Grab next byte and prepare space
    0x28 or uR1 uR1 MDR; none           # Incorporate reg byte into return

    0x29 mov uPC uRet; none             # Return

# FetchOpIdx:
    # Skip Grabbing the first register byte if not specified
    0x2A if nBits MDR 0x0C FetchOpIdx2
    0x2B mov uPC uRet; none             # Return

    0x2C if pcmax halt                  # Halt on PC overflow
    0x2D add PC PC i1; none             # Increment PC

    0x2E mov mar pc; none               # Target next byte
    0x2F read; sll uR1 uR1 iB           # Grab next byte and prepare space
    0x30 or uR1 uR1 MDR; none           # Incorporate reg byte into return

# FetchOpIdx2:
    0x31 none; sll uR1 uR1 iB           # Move up read byte
    0x32 if nBits uR0 0x03 uRet         # Possibly skip mem read
    
    0x33 if pcmax halt                  # Halt on PC overflow
    0x34 add PC PC i1; none             # Increment PC

    0x35 mov MAR PC; none               # Target Next Byte
    0x36 read; none                     # Grab the next byte
    0x37 or uR1 uR1 MDR; none           # Incorporate byte into return

    0x38 mov uPC uRet; none             # Return

# FetchOpDsp:
    0x39 and uR0 uR0 i5; mov uCnt i0    # Clear uR0 MSB and count register

    # Skip grabbing Reg byte if not specified
    0x3A if bits MDR 0x0C FetchOpImm

    0x3B if pcmax Halt                  # Halt on PC overflow
    0x3C add PC PC i1; none             # Increment PC

    0x3D mov MAR PC; none               # Target the next byte
    0x3E read; none                     # Read in the next byte
    0x3F mov uCnt MDR; none             # Store the register byte in a temp reg
    
    0x40 sll uCnt uCnt iF; none         # Shift the register byte up three bytes
    0x41 or uR0 uR0 uCnt; none          # Store the register byte in uR0

    0x42 none; goto FetchOpImm          # Grab the immediate for this mode

# FetchOpScl:
    0x43 mov uR2 MDR; and uR0 uR0 i2    # Move scale param to uR2 and clear it 
                                        # from uR0
    0x44 sll uR2 uR2 iB; none           # Make space in uR2 for the reg byte

    0x45 if bits MDR 0x03 FetchOpScl1

    0x46 if pcmax Halt                  # Halt on PC overflow
    0x47 add PC PC i1; none             # Increment the PC

    0x48 mov mar pc; none               # Target the next byte
    0x49 read; none                     # Read the register byte
    0x4A or uR2 uR2 MDR; none           # Store the register byte

# FetchOpScl1:
    0x4B if pcmax Halt                  # Halt on PC overflow
    0x4C add PC PC i1; none             # Increment the PC

    0x4D mov MAR PC; none               # Target the next byte
    0x4E read; none                     # Grab the next byte

    0x4F if nyb MDR 0x00 halt           # Fail on prefix byte
    0x50 if nyb MDR 0xB0 halt           # Fail on scale of scale

    # Store op type in reg and decode scaled op 
    0x51 or uR0 uR0 MDR; goto FetchOpVal

