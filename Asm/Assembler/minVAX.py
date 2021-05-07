from parser import *

mnemonics = {'HLT':0x0, 'ADD':0x1, 'AND':0x2, 'SRA':0x3, 'SLL':0x4, 'LDR':0x5, 'STR':0x6, 'JMP':0x7, 
             'BEZ':0x8, 'BLT':0x9, 'NOP':0xA, 'CLR':0xB, 'CMP':0xC, 'INC':0xD, 'DMP':0xE}

noRA = {'HLT', 'JMP', 'NOP'}
noAM = {'HLT', 'NOP', 'CLR', 'CMP', 'INC', 'DMP'}

def instrSizeFxn(instrToks):
    global mnemonics, noRA, noAM
    
    if len(instrToks) == 0:
        return 0
    
    mnemonic, args = getArgs(instrToks)
    mnemonic = mnemonic.upper()
    
    # Without an AM, this instruction can't have an immediate word following
    if mnemonic in noAM:
        return 1
    
        
    if args[-1][0] == 'REG':
        return 1
    
    return 2

def printArgs(toks):
    idx = 0
    
    codeSeg = True
    
    lineToks = []
    while idx < len(toks):
        try:
            tok = toks[idx]
            
            if tok.tokType == 'DATA_SEG':
                codeSeg = False
                continue
                
            if tok.tokType == 'CODE_SEG':
                codeSeg = True
                continue
            
            if not codeSeg or tok.tokType == 'LABEL':
                continue
        
            if tok.tokType == 'EOL' or tok.tokType == 'END_TOKS':
                if len(lineToks) > 0:
                    print(getArgs(lineToks))
                    lineToks = []
            else:
                lineToks.append(tok)
        finally:
            idx += 1

def getAMEnc(arg):
    am = 0
    imm = None
    
    if arg[0] == 'REG':
        if int(arg[1]) >= 0 and int(arg[1]) <= 1:
            am = int(arg[1])
        else:
            printErr("Register operands can only 0 or 1", None)
            exit(1)
            
    elif arg[0] == 'DSP':
        imm = arg[1]
        rNum = int(arg[2])
        if rNum >= 0 and rNum <= 1:
            am = rNum + 2
        elif rNum == 2:
            am = 6
        else:
            printErr("Displacement only on regs 0 and 1, PCR only on reg 2", None)
            exit(1)
    
    elif arg[0] == 'IMM':
        imm = arg[1]
        am = 4
        
    elif arg[0] == 'ABS':
        imm = arg[1]
        am = 5
    
    return am, imm

def getInstLine(instr, codePos):
    global mnemonics, noRA, noAM
    
    mnemonic = instr[0].upper()
    args = instr[1]
    opcByte = mnemonics[mnemonic] << 4
    immByte = None
    width = 1
    
    if mnemonic in noRA and mnemonic in noAM:
        pass
    elif mnemonic in noRA:
        am, immByte = getAMEnc(args[0])
        opcByte += am << 1
        
    elif mnemonic in noAM:
        if args[0][0] != 'REG':
            printErr("First arg must be a register parameter", None)
            exit(1)
            
        regNr = int(args[0][1])
        if regNr != 0 and regNr != 1:
            printErr("RA must be 0 or 1", None)
            exit(1)
        
        opcByte += regNr
        
    else:
        if args[0][0] != 'REG':
            printErr("First arg must be a register parameter", None)
            exit(1)
            
        regNr = int(args[0][1])
        if regNr != 0 and regNr != 1:
            printErr("RA must be 0 or 1", None)
            exit(1)
        
        am, immByte = getAMEnc(args[1])
        
        opcByte += regNr
        opcByte += am << 1
        
    
    if immByte is not None:
        width = 2
    
    line = hex(codePos)[2:] + ' ' + hex(width)[2:] + ' ' + hex(opcByte)[2:]
    if immByte is not None:
        line += ' ' + hex(immByte)[2:]
    
    return line, width
    

def encode(instrs, dataInit, codeBase, dataBase):
    codePos = codeBase
    
    for instr in instrs:
        line, nWords = getInstLine(instr, codePos)
        print(line)
        codePos += nWords
    
    for i in range(0, len(dataInit)):
        value, width, offset = dataInit[i]
        print(hex(dataBase + offset)[2:] + ' ' + hex(width)[2:], end='')
        
        for j in range(0, width):
            print(' ' + hex(value & 0xFF)[2:], end='')
            value = value << 8
        print()
    print(hex(codeBase)[2:])
    

def main():
    toks = clean()
    dataLabels, dataInit = dataOffsets(toks)

    codeLabels, codePos = codeOffsets(toks, instrSizeFxn)
    instrs = getInstrs(toks, 0, codeLabels, codePos, dataLabels)
    
    #printArgs(toks)
    #print('Data Labels:' + str(dataLabels))
    #print(dataInit)
    #print('Code Labels:' + str(codeLabels))
    #print(codePos)
    
    #for instr in instrs:
        #print(instr)
        
    #print()
    encode(instrs, dataInit, 0, codePos)
    

main()
