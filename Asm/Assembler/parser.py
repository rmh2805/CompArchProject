from sys import exit, stdin

class Tok:
    def __init__(self, st, lineNr=None):
        data = st.split(maxsplit=2)
        self.tokType = data[0]
        self.val = None
        if len(data) == 2:
            self.val = data[1]
        
        self.lineNr = lineNr
        if self.tokType == 'EOL':
            self.lineNr -= 1
            
    def __str__(self):
        return 'Tok{tokType:\'' + str(self.tokType) + '\', val:\'' + str(self.val) + '\', lineNr:' + str(self.lineNr) +'}'
    
    def __repr__(self):
        return str(self)

def align(curDisp, boundary):
    if(curDisp % boundary == 0):
        return curDisp
    else:
        return curDisp + boundary - curDisp % boundary

def printErr(reason, lineNr):
    print("Error in line " + str(lineNr) + ": " + reason + ".")

# Takes in a list of tokens from stdin and cleans it (removes extra EOL tokens)
# Returns a list of tokens
def clean():
    toks = list()
    
    lineNr = 1
    
    lastEOL = False
    for line in stdin:
        line = line.strip()
        if line == "EOL":
            lineNr += 1
            if lastEOL:
                continue
            lastEOL = True
        else:
            lastEOL = False

        toks.append(Tok(line, lineNr))
    
    toks.append(Tok("END_TOKS")) # dummy included here to mark end of toks without worry for overflow
    
    return toks

def isValType(tok):
    return tok.tokType == 'INT' or tok.tokType == 'WORD'

def parseArg(argToks):
    tokType = argToks[-1].tokType
    # Check for scale arg (recursive case)
    if tokType == 'CLOSE_BRACKET':
        if len(argToks) < 5:
            printErr("Wrong number of tokens for a scale operand", argToks[0].lineNr)
            exit(1)
        
        if argToks[-4].tokType != 'OPEN_BRACKET' or argToks[-3].tokType != 'REG_MARK' or not isValType(argToks[-2]):
            printErr("Malformed scale operand", argToks[0].lineNr)
            exit(1)
        
        subArg = parseArg(argToks[:-4])
        if subArg[0] == 'SCL':
            printErr("Cannot scale a scale operand", argToks[0].lineNr)
            exit(1)
        
        
        return ("SCL", subArg, argToks[-2])
    
    
    # Increment operand
    if tokType == 'PLUS_SIGN':
        if len(argToks) != 5:
            printErr("Wrong number of tokens for increment operand", argToks[0].lineNr)
            exit(1)
        
        if argToks[0].tokType != 'OPEN_PAREN' or argToks[1].tokType != 'REG_MARK' or not isValType(argToks[2]) or argToks[3].tokType != 'CLOSE_PAREN':
            printErr("Malformed increment operand", argToks[0].lineNr)
            exit(1)
        
        return ("INC", argToks[2])
        
    
    tokType = argToks[0].tokType
    # Register operand
    if tokType == "REG_MARK":
        if len(argToks) != 2 or not isValType(argToks[1]):
            printErr("Invalid register operand", argToks[0].lineNr)
            exit(1)
            
        return ("REG", argToks[1])
    
    # Immediate operand
    if tokType == "IMM_MARK":
        if len(argToks) != 2 or not isValType(argToks[1]):
            printErr("Invalid immediate operand", argToks[0].lineNr)
            exit(1)
        
        return ("IMM", argToks[1])
        
    # Decrement operand
    if tokType == 'MINUS_SIGN':
        if len(argToks) != 5:
            printErr("Wrong number of tokens for decrement operand", argToks[0].lineNr)
            exit(1)
        
        if argToks[1].tokType != 'OPEN_PAREN' or argToks[2].tokType != 'REG_MARK' or not isValType(argToks[3]) or argToks[4].tokType != 'CLOSE_PAREN':
            printErr("Malformed decrement operand", argToks[0].lineNr)
            exit(1)
        
        return ("DEC", argToks[3])
        
    
    # Memory indirect operand
    if tokType == "AT_SIGN":
        if len(argToks) != 4: 
            printErr("Wrong number of tokens for mem. ind. operand", argToks[0].lineNr)
            exit(1)
        
        if argToks[1].tokType != 'OPEN_PAREN' or not isValType(argToks[2]) or argToks[3].tokType != 'CLOSE_PAREN':
            printErr("Invalid memory inderect operand", argToks[0].lineNr)
            exit(1)
            
        return ("IDM", argToks[2])
    
    # Reg indirect and index operands
    if tokType == 'OPEN_PAREN':
        nToks = len(argToks)
        
        if nToks != 4 and nToks != 7:
            printErr("Wrong # of tokens for register inderect or index operand", argToks[0].lineNr)
            exit(1)
        elif nToks == 4:
            if argToks[1].tokType != 'REG_MARK' or not isValType(argToks[2]) or argToks[3].tokType != 'CLOSE_PAREN':
                printErr('Malformed register indirect operand', argToks[0].lineNr)
                exit(1)
            
            return ('IDR', argToks[2])
        else:
            if argToks[1].tokType != 'REG_MARK' or not isValType(argToks[2]) or argToks[3].tokType != 'PLUS_SIGN':
                printErr('Malformed index operand', argToks[0].lineNr)
                exit(1)
            if argToks[4].tokType != 'REG_MARK' or not isValType(argToks[5]) or argToks[6].tokType != 'CLOSE_PAREN':
                printErr('Malformed index operand', argToks[0].lineNr)
                exit(1)
            
            return ('IDX', argToks[2], argToks[5])
    
    # Absolute and displacement operands
    if isValType(argToks[0]):
        if len(argToks) == 1:
            return ("ABS", argToks[0])
            
        if len(argToks) != 5:
            printErr("Wrong number of args for a displacement operand", argToks[0].lineNr)
            exit(1)
            
        if argToks[1].tokType != 'OPEN_PAREN' or argToks[2].tokType != 'REG_MARK' or not isValType(argToks[3]) or argToks[4].tokType != 'CLOSE_PAREN':
            printErr("Malformed displacement operand", argToks[0].lineNr)
            exit(1)
            
        return ("DSP", argToks[0], argToks[3])
    
    return None

# Takes in a list of tokens for a single line and outputs a parse of its mnemonic and arg types
def getArgs(lineToks):
    tok = lineToks[0]
    
    if tok.tokType != 'WORD':
        printErr("Line doesn't begin with an instruction mnemonic", tok.lineNr)
        exit(1)
    
    mnemonic = tok.val
    args = []
    
    codeSeg = True
    argToks = []
    
    idx = 1
    while idx < len(lineToks):
        try:
            tok = lineToks[idx]
            tokType = tok.tokType
            
        finally:
            if tokType != 'COMMA':
                argToks.append(tok)
            
            if tokType == 'COMMA' or idx == len(lineToks) - 1:
                args.append(parseArg(argToks))
                argToks = []
            
            idx += 1
        
    
    return (mnemonic, args)

# Takes in a list of tokens and returns a dictionary from labels onto data segment offsets and 
# A list of 3-tuples [(value, width, offset)] for data segment initialization
def dataOffsets(toks):
    dataLabels = dict()
    dataInit = list()
    dataPos = 0
    
    codeSeg = True
    curLabels = []
    
    width = 1
    
    idx = 0
    while idx < len(toks):
        try:
            tok = toks[idx]
            
            realigned = False
            
            if tok.tokType == 'DATA_SEG':
                codeSeg = False
                continue
            
            if tok.tokType == 'CODE_SEG':
                codeSeg = True
                continue
            
            if codeSeg:
                continue
            
            if tok.tokType == 'LABEL':
                if tok.val in dataLabels:
                    printErr("Attempt to redefine a label", tok.lineNr)
                    exit(1)
                
                curLabels.append(tok.val)
                dataLabels[tok.val] = dataPos
                
                continue
            
            if tok.tokType == 'WORD_ALIGN':
                dataPos = align(dataPos, 4)
                realigned = True
                width = 4
                continue
            
            if tok.tokType == 'HALF_ALIGN':
                dataPos = align(dataPos, 2)
                realigned = True
                width = 2
                continue
            
            if tok.tokType == 'BYTE_ALIGN':
                realigned = True
                width = 1
                continue
                
            if tok.tokType == 'INT':
                dataInit.append((int(tok.val), width, dataPos))
                dataPos += width
                
            
            
        finally:
            if realigned:
                for label in curLabels:
                    dataLabels[label] = dataPos
                
                if toks[idx + 1].tokType != 'INT':
                    dataInit.append((0, width, dataPos))
                
                curLabels = list()
            
            if tok.tokType == 'INT':
                curLabels = list()
            
            if tok.tokType == 'EOL' or tok.tokType == 'END_TOKS':
                width = 1
            
            idx += 1
    
    return dataLabels, dataInit

# Takes in a list of tokens, an instruction size calculator, and (optionally) an instruction 
# alignment, and returns a dictionary from labels onto code segment offsets and the next available 
# code segment offset
def codeOffsets(toks, instrSizeFxn, instrAlignment=1):
    codeLabels = dict()
    codePos = 0
    
    lineToks = []
    codeSeg = True
    
    idx = 0
    while idx < len(toks):
        try:
            tok = toks[idx]
            
            if tok.tokType == 'CODE_SEG':
                codeSeg = True
                continue
                
            if tok.tokType == 'DATA_SEG':
                codeSeg = False
                continue
                
            if not codeSeg:
                continue
            
            if tok.tokType == 'EOL' or tok.tokType == 'END_TOKS':
                codePos += instrSizeFxn(lineToks)
                lineToks = []
                continue
            
            if tok.tokType == 'LABEL':
                if tok.val in codeLabels:
                    printErr("Attempt to overwrite label '" + tok.val + "'", tok.lineNr)
                    exit(1)
                
                codeLabels[tok.val] = codePos
                continue
                
            lineToks.append(tok)
            
        finally:
            
            idx += 1
    
    return codeLabels, align(codePos, 4)

def evalVal(tok, codeBase, codeLabels, dataBase, dataLabels):
    if tok.tokType == 'INT':
        return int(tok.val)
    if tok.val in codeLabels:
        return codeBase + codeLabels[tok.val]
    return dataBase + dataLabels[tok.val]

def evalArg(arg, codeBase, codeLabels, dataBase, dataLabels):
    argType = arg[0]
    
    if argType == 'SCL':
        return (argType, evalArg(arg[1], codeBase, codeLabels, dataBase, dataLabels), evalVal(arg[2], codeBase, codeLabels, dataBase, dataLabels))
    
    if argType == 'DSP' or argType == 'IDX':
        return (argType, evalVal(arg[1], codeBase, codeLabels, dataBase, dataLabels), evalVal(arg[2], codeBase, codeLabels, dataBase, dataLabels))
        
    return (argType, evalVal(arg[1], codeBase, codeLabels, dataBase, dataLabels))
    

def getInstrs(toks, codeBase, codeLabels, dataBase, dataLabels):
    instrs = list()
    
    codeSeg = True
    lineToks = []
    
    idx = 0
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
                    instrs.append(getArgs(lineToks))
                    lineToks = []
            else:
                lineToks.append(tok)
        finally:
            idx += 1
    
    for i in range(0, len(instrs)):
        mnemonic, args = instrs[i]
        
        for j in range(0, len(args)):
            args[j] = evalArg(args[j], codeBase, codeLabels, dataBase, dataLabels)
        
        instrs[i] = (mnemonic, args)
            
    
    return instrs
    
