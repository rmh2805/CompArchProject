from parser import *
from sys import exit

mnemonics = {'HLT':0x00, 'NOP':0x01, 'CLR':0x02, 'DMP':0x03,
             'OUT':0x04, 
             
             'ADD':0x10, 'SUB':0x11, 'MUL':0x12, 'DIV':0x13,
             'MOD':0x14, 'AND':0x15,  'OR':0x16, 'XOR':0x17,
             'CMP':0x18, 'INC':0x19, 'DEC':0x1A,
             
             'MOV':0x20, 'SWP':0x21, 'CPY':0x22,
             
                         'JMP':0x31, 'JAL':0x32, 'RET':0x33,
             'BEQ':0x34, 'BNE':0x35, 'BGT':0x36, 'BGE':0x37,
             'BIE':0x38, 'BDE':0x39}

maxArgs   = {'HLT':0, 'NOP':0, 'CLR':1, 'DMP':1,
             'OUT':2, 
             
             'ADD':3, 'SUB':3, 'MUL':4, 'DIV':4,
             'MOD':3, 'AND':3,  'OR':3, 'XOR':3,
             'CMP':2, 'INC':1, 'DEC':1,
             
             'MOV':2, 'SWP':2, 'CPY':4,
             
                         'JMP':1, 'JAL':1, 'RET':0,
             'BEQ':3, 'BNE':3, 'BGT':3, 'BGE':3,
             'BIE':3, 'BDE':3}

regNrs = {'R0':0x00, 'R1':0x01, 'R2':0x02, 'R3':0x03,
          'R4':0x04, 'R5':0x05, 'R6':0x06, 'R7':0x07,
          'R8':0x08, 'R9':0x09, 'RA':0x0A, 'RB':0x0B,
          'PR':0x0C, 'PB':0x0D, 'PS':0x0E, 'PC':0x0F,

          'r0':0x00, 'r1':0x01, 'r2':0x02, 'r3':0x03,
          'r4':0x04, 'r5':0x05, 'r6':0x06, 'r7':0x07,
          'r8':0x08, 'r9':0x09, 'rA':0x0A, 'rB':0x0B,
          'pR':0x0C, 'pB':0x0D, 'pS':0x0E}

def argSize(arg):
    global regNrs
    argType = arg[0]
    arg = arg[1:]

    if argType == 'REG' or argType == 'IDR' or argType == 'INC' or argType == 'DEC':
        # Get the register number
        rNr = arg[0].val
        if rNr in regNrs:
            rNr = regNrs[rNr]
        
        # Return 1 on quick register, 2 on slow register
        if rNr == 0x00 or rNr == 0x01 or rNr == 0x0D:
            return 1
        return 2
    if argType == 'IMM' or argType == 'ABS' or argType == 'IDM':
        return 5    # Return max size
    if argType == 'IDX':
        sz = 1

        # Get the first register's number
        rNr = arg[0].val
        if rNr in regNrs:
            rNr = regNrs[rNr]
        
        # Increment size for slow register
        if not(rNr == 0x00 or rNr == 0x01 or rNr == 0x0D):
            sz += 1

        # Get the second register's number
        rNr = arg[1].val
        if rNr in regNrs:
            rNr = regNrs[rNr]
        
        # Increment size for slow register
        if not(rNr == 0x00 or rNr == 0x01 or rNr == 0x0D):
            sz += 1
        
        return sz
        
    if argType == 'DSP':
        sz = 5  # Basic size of lead-in and immediate
        # Get the register's number
        rNr = arg[1].val
        if rNr in regNrs:
            rNr = regNrs[rNr]
        
        # Increment size for slow register
        if not(rNr == 0x00 or rNr == 0x01 or rNr == 0x0D):
            sz += 1
        
        return sz



    print(argType + ": " + str(arg))
    return 1

def instrSizeFxn(instrToks):
    global mnemonics, maxArgs

    if len(instrToks) == 0:
        return 0
    
    mnemonic, args = getArgs(instrToks)
    mnemonic = mnemonic.upper()

    if mnemonic not in mnemonics:
        print("ERROR in line " + str(instrToks[0].lineNr) + ": unknown mnemonic " + str(mnemonic))
        exit(1)

    argMax = maxArgs[mnemonic]

    sz = 1                  # Argument byte
    if len(args) < argMax:
        sz += 1             # End of args byte

    for arg in args:
        sz += argSize(arg)

    return sz

def encodeImm(val):
    argStr = ''
    
    for i in range(0, 4):
        argStr += ' ' + hex(val & 0xFF)[2:]
        val = val >> 8
    
    return argStr

def encodeReg(argByte, rNr):
    argStr = ''
    nWords = 0

    if rNr == 0:
        argByte = argByte | 0x00
    elif rNr == 1:
        argByte = argByte | 0x04
    elif rNr == 0x0D:
        argByte = argByte | 0x08
    else:
        argByte = argByte | 0x0C
        argStr = ' ' + hex(rNr)[2:]
        nWords += 1
    
    argStr = hex(argByte)[2:] + argStr
    nWords += 1

    return argStr, nWords


def encodeArg(arg, incAmt = 4):
    argStr = ""
    nWords = 0

    if arg[0] == 'REG':
        argStr, nWords = encodeReg(0x33, arg[1] - 1)
    
    elif arg[0] == 'IMM':
        argStr = '23'
        nWords += 5
        argStr += encodeImm(arg[1])
    
    elif arg[0] == 'ABS':
        argStr = '83'
        nWords += 5
        argStr += encodeImm(arg[1])

    elif arg[0] == 'IDR':
        argStr, nWords = encodeReg(0x43, arg[1] - 1)

    elif arg[0] == 'IDM':
        argStr = '53'
        nWords += 5
        argStr += encodeImm(arg[1])
    
    elif arg[0] == 'IDX':
        argStr = ''
        argByte = 0x60

        rNr = arg[1] - 1
        if rNr == 0:
            argByte = argByte | 0x00
        elif rNr == 1:
            argByte = argByte | 0x04
        elif rNr == 0x0D:
            argByte = argByte | 0x08
        else:
            argByte = argByte | 0x0C
            argStr = ' ' + hex(rNr)[2:]
            nWords += 1
        
        rNr = arg[2] - 1
        if rNr == 0:
            argByte = argByte | 0x00
        elif rNr == 1:
            argByte = argByte | 0x01
        elif rNr == 0x0D:
            argByte = argByte | 0x02
        else:
            argByte = argByte | 0x03
            argStr = ' ' + hex(rNr)[2:]
            nWords += 1
        
        argStr = hex(argByte)[2:] + argStr
        nWords += 1

    elif arg[0] == 'DSP':
        argStr, nWords = encodeReg(0x73, arg[2] - 1)
        argStr += encodeImm(arg[1])
        nWords += 4

    elif arg[0] == 'INC':
        argByte = 0x90 | (incAmt - 1)
        argStr, nWords = encodeReg(argByte, arg[1] - 1)
    elif arg[0] == 'DEC':
        argByte = 0xA0 | (incAmt - 1)
        argStr, nWords = encodeReg(argByte, arg[1] - 1)
    else:
        print(arg)

    return argStr, nWords

def encodeInst(inst, codePos):
    global mnemonics, maxArgs
    line = ""
    nWords = 1
    
    mnemonic = inst[0].upper()
    args = inst[1]

    # Insert the mnemonic into the instruction
    if mnemonic not in mnemonics:
        print("ERROR: Unrecognized mnemonic " + mnemonic)
        exit(-1)
    line = hex(mnemonics[mnemonic])[2:]

    # Insert each arg into the instruction
    for arg in args:
        argStr, argWords = encodeArg(arg)
        line += ' ' + argStr
        nWords += argWords

    # Insert explicit end of args
    if maxArgs[mnemonic] != len(args):
        line += ' 0'
        nWords += 1

    # Apply the positioning information
    line = hex(codePos)[2:] + ' ' + hex(nWords)[2:] + ' ' + line
    return line, nWords
    

def encode(instrs, dataInit, codeBase, dataBase):
    codePos = codeBase
    for inst in instrs:
        line, nWords = encodeInst(inst, codePos)
        print(line)
        codePos += nWords

    for i in range(0, len(dataInit)):
        value, width, offset = dataInit[i]
        print(hex(dataBase + offset)[2:] + ' ' + hex(width)[2:], end='')
        
        for j in range(0, width):
            print(' ' + hex(value & 0xFF)[2:], end='')
            value = value >> 8
        print()
    print(hex(codeBase - 1)[2:])

def main():
    toks = clean()
    dataLabels, dataInit = dataOffsets(toks)
    codeLabels, codePos = codeOffsets(toks, instrSizeFxn)
    codePos = codePos + 4

    for codeLabel in codeLabels:
        codeLabels[codeLabel] -= 1

    for reg in regNrs:
        if reg in codeLabels or reg in dataLabels:
            print("Register mnemonic " + reg + " overrides existing label")
            return -1
        codeLabels[reg] = regNrs[reg]

    instrs = getInstrs(toks, 1, codeLabels, codePos, dataLabels)
    encode(instrs, dataInit, 1, codePos)

main()
