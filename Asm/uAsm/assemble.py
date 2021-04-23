from sys import exit, stdin, argv

totLineNr = 1

class Tok:
    def __init__(self, st, lineNr=None):
        data = st.split(maxsplit=2)
        self.tokType = data[0].upper()
        self.val = None
        if len(data) == 2:
            self.val = data[1].lower()
        
        self.lineNr = lineNr
        if self.tokType == 'EOL':
            self.lineNr -= 1
            
    def __str__(self):
        return 'Tok{tokType:\'' + str(self.tokType) + '\', val:\'' + str(self.val) + '\', lineNr:' + str(self.lineNr) +'}'
    
    def __repr__(self):
        return str(self)

class Inst:
    def __init__(self):
        self.offset = None
        
        self.type = None
        
        self.opA = None
        self.regA = [0, 0, 0]
        
        self.opB = None
        self.regB = [0, 0, 0]
        
        self.condition = None
        self.condArgs = [0, 0]
        
        self.address = None
        
        self.relocLast = False
    
    def __str__(self):
        return 'Inst{offset:' + str(self.offset) + ', type:' + str(self.type) + ', '\
               'opA:' + str(self.opA) + ', regA:' + str(self.regA) + ', opB:' + str(self.opB) + \
               ', regB:' + str(self.regB) + ', condition:' + str(self.condition) + \
               ', condArgs:' + str(self.condArgs) + ', address:' + str(self.address) +'}'
    
    def __repr__(self):
        return str(self)
    
    def encode(self):
        if self.type is None:
            return None
        
        encoding = hex(self.offset)[2:] + ' 1 '
        val = self.type << 53
        if self.type == 0:
            val |= self.opA << 50
            val |= self.regA[0] << 44
            val |= self.regA[1] << 38
            val |= self.regA[2] << 32
            
            val |= self.opB << 29
            val |= self.regB[0] << 23
            val |= self.regB[1] << 17
            val |= self.regB[2] << 11
        else:
            val |= self.address
            if self.type == 1:
                val |= self.opA << 50
                val |= self.regA[0] << 44
                val |= self.regA[1] << 38
                val |= self.regA[2] << 32
            elif self.type == 2:
                val |= self.opB << 50
                val |= self.regB[0] << 44
                val |= self.regB[1] << 38
                val |= self.regB[2] << 32
            elif self.type == 3:
                val |= self.condition << 32
                val |= self.condArgs[0] << 44
                val |= self.condArgs[1] << 38
        
        encoding += hex(val)[2:]
        return encoding

def printErr(cause, lineNr):
    print('ERROR on line ' + str(lineNr) + ': ' + str(cause))

def getLine():
    global totLineNr

    toks = list()
    
    for line in stdin:
        
        line = line.strip()
        
        if line == "EOL":
            totLineNr += 1
            
            if len(toks) != 0:
                return toks
            
            continue
        
        toks.append(Tok(line, totLineNr))

    return None

regMap = {'i0': 0x00, 'i1': 0x01, 'i2': 0x02, 'i3': 0x03,
          'i4': 0x04, 'i5': 0x05, 'i6': 0x06, 'i7': 0x07,
          'i8': 0x08, 'i9': 0x09, 'ia': 0x0A, 'iB': 0x0B,
          'ic': 0x0C, 'iD': 0x0D, 'ie': 0x0E, 'iF': 0x0F,
          
          'r0': 0x30, 'r1': 0x31, 'r2': 0x32, 'r3': 0x33,
          'r4': 0x34, 'r5': 0x35, 'r6': 0x36, 'r7': 0x37,
          'r8': 0x38, 'r9': 0x39, 'ra': 0x3A, 'rb': 0x3B,
          'pr': 0x3C, 'pb': 0x3D, 'ps': 0x3E, 'pc': 0x3F,
          
          'upc':0x10, 'uret':0x11, 'ur0':0x12, 'ur1':0x13,
          'ur2':0x14, 'ucnt':0x15, 'mdr':0x16, 'mar':0x17,
          'op1val':0x18, 'op1type':0x19, 'op2val':0x1A, 'op2type':0x1B,
          'op3val':0x1C, 'op3type':0x1D, 'op4val':0x1E, 'op4type':0x1F,
          'op1scale':0x20, 'op2scale':0x21, 'op3scale':0x22, 'op4scale':0x23,
          'ir':0x24, 'utmp':0x25, 'uret2':26
          }

immRegs = range(0x00, 0x0F + 1)

opAMap = {'none' :(0, 0), 'mov' :(1, 2),
          'sll'  :(2, 3), 'srl' :(3, 3),
          'or'   :(4, 3), 'ext' :(5, 2),
          'xor'  :(6, 3), 'and' :(7, 3)}

opBMap = {'none' :(0, 0), 'mov' :(1, 2),
          'cmp'  :(2, 2), 'read':(3, 0),
          'write':(4, 0), 'sub' :(5, 3),
          'add'  :(6, 3)}

matchOp = {0, 1}

condMap = {'eq'   :(0x0, 1), 
           'neq'  :(0x1, 1), 
           'bits' :(0x2, 2),
           'nbits':(0x3, 2),
           'byte' :(0x4, 2),
           'nbyte':(0x5, 2),
           'nyb'  :(0x6, 2),
           'nnyb' :(0x7, 2),
           
           'noops':(0x8, 0),
           'oneop':(0x9, 0),
           'twoops':(0xA, 0),
           'threeops':(0xB, 0),
           'pcmax':(0xC, 0)}

loadPoints = dict()

def doCond(inst, line):
    line = line[1:]
    inst.type = 3
    
    if line[0].val not in condMap:
        printErr('Unrecognized condition type', line[0].lineNr)
        exit(1)
    
    condType, operandType = condMap[line[0].val]
    inst.condition = condType
    
    if line[-1].tokType != 'INT':
        if line[-1].val not in loadPoints:
            printErr('Non-integer conditional destination', line[-1].lineNr)
            exit(1)
        inst.address = loadPoints[line[-1].val]
    else:
        inst.address = int(line[-1].val)
    
    if operandType == 0:    # These are the no operand conditionals
        return
    
    if line[1].val not in regMap:
        printErr('Unrecognized register \'' + line[1].val + '\'' + line[1].lineNr)
        exit(1)
    
    inst.condArgs[1] = regMap[line[1].val]
    if operandType == 1:
        if line[2].val not in regMap:
            printErr('Unrecognized register \'' + line[2].val + '\'' + line[2].lineNr)
            exit(1)
        iReg = regMap[line[2].val]
            
        if iReg not in immRegs:
            printErr('Non-immediate register \'' + line[2].val + '\' in conditional', line[2].lineNr)
            exit(1)
        
        inst.condArgs[0] = iReg
        return
    
    if operandType == 2:
        if line[2].tokType != 'INT':
            printErr('Non-immediate arg \'' + line[2].val + '\' in conditional', line[2].lineNr)
            exit(1)
        
        byteImm = int(line[2].val)
        if byteImm != byteImm & 0xFF:
            printErr('Non-byte immediate \'' + line[2].val + '\' in conditional', line[2].lineNr)
            exit(1)
            
        
        inst.condArgs[0] = int(line[2].val)
        return
    
def getLoadPoints(fName):
    fp = open(fName, 'r')
    lines = fp.readlines()
    fp.close()
    
    for i in range(0, len(lines)):
        data = lines[i].strip().split()
        
        if len(data) != 2:
            printErr("Malformed line from load point file", i+1)
            exit(1)
        
        loadPoints[data[0].lower()] = int(data[1], 16)

def doOpA(inst, line):
    if inst.opA is not None:
        if line[0].val in opBMap:
            return doOpB(inst, line)
        
        if inst.opA not in matchOp:
            printErr('Attempted to perform 2 A field instructions in one line', line[0].lineNr)
            exit(1)
        
        inst.opB = inst.opA
        inst.regB = inst.regA
        inst.regA = [0, 0, 0]
    
    opc, nRegs = opAMap[line[0].val] 
    line = line[1:]
    inst.opA = opc
    
    return doOpRegs(line, nRegs, inst.regA)
    
def doOpB(inst, line):
    if inst.opB is not None:
        if line[0].val in opAMap:
            return doOpA(inst, line)
            
        if inst.opB not in matchOp:
            printErr('Attempted to perform 2 B field instructions in one line', line[0].lineNr)
            exit(1)
        
        inst.opA = inst.opB
        inst.regA = inst.regB
        inst.regB = [0, 0, 0]
    
    
    opc, nRegs = opBMap[line[0].val] 
    line = line[1:]
    inst.opB = opc
    
    return doOpRegs(line, nRegs, inst.regB)
    
def doOpRegs(line, nRegs, regs):
    for i in range(0, nRegs):
        if line[0].val not in regMap:
            printErr('Unrecognized register \'' + line[0].val + '\'' + line[0].lineNr)
            exit(1)
        
        regs[i] = regMap[line[0].val]
        line = line[1:]
    return line

def main():
    if len(argv) > 1:
        getLoadPoints(argv[1])
    
    insts = list()
    
    line = getLine()
    while line is not None:
        inst = Inst()
        
        # Ensure an offset is included
        if line[0].tokType != 'INT':
            printErr('Line starts with non-int value', line[0].lineNr)
            exit(1)
        
        inst.offset = int(line[0].val)
        line = line[1:]
        
        # Catch conditionals here
        if line[0].tokType != 'WORD':
            printErr('No command mnemonic', line[0].lineNr)
            exit(1)
        
        if line[0].val == 'if':
            doCond(inst, line)
        else:
            # Handle the first field
            if line[0].val in opAMap:
                line = doOpA(inst, line)
            elif line[0].val in opBMap:
                line = doOpB(inst, line)
            else:
                printErr('Unrecongized mnemonic \'' + str(line[0].val) + '\'', line[0].lineNr)
                exit(1)
            
            # Handle the second field
            if line[0].val == 'goto':
                if line[-1].tokType != 'INT':
                    if line[-1].val not in loadPoints:
                        printErr('Non-integer conditional destination', line[-1].lineNr)
                        exit(1)
                    inst.address = loadPoints[line[-1].val]
                else:
                    inst.address = int(line[-1].val)
            elif line[0].val in opAMap:
                line = doOpA(inst, line)
            elif line[0].val in opBMap:
                line = doOpB(inst, line)
            else:
                printErr('Unrecongized mnemonic \'' + str(line[0].val) + '\'', line[0].lineNr)
                exit(1)
            
            # Set address type
            if inst.opA is not None and inst.opB is not None and inst.address is None:
                inst.type = 0
            elif inst.opA is not None and inst.opB is None and inst.address is not None:
                inst.type = 1
            elif inst.opA is None and inst.opB is not None and inst.address is not None:
                inst.type = 2
            else:
                printErr('Invalid command mix (must be A B, A goto, or B goto)')
                exit(1)
            
            
                
        
        print(inst.encode())
        
        # Save instruction and grab the next line
        insts.append(inst)
        line = getLine()
    
    return None

main()
