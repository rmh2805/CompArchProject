#include "cpu.h"

//===============================<CPU Objects>================================//
BusALU ALU1("ALU1", kAddrBusSize);
BusALU ALU2("ALU2", kAddrBusSize);

// General Purpose Register File
StorageObject * GPR[16] = {
    new StorageObject("R0", kRegSize),
    new StorageObject("R1", kRegSize),
    new StorageObject("R2", kRegSize),
    new StorageObject("R3", kRegSize),
    new StorageObject("R4", kRegSize),
    new StorageObject("R5", kRegSize),
    new StorageObject("R6", kRegSize),
    new StorageObject("R7", kRegSize),
    new StorageObject("R8", kRegSize),
    new StorageObject("R9", kRegSize),
    new StorageObject("RA", kRegSize),
    new StorageObject("RB", kRegSize),
    new StorageObject("pR", kRegSize),
    new StorageObject("pB", kRegSize),
    new StorageObject("pS", kRegSize),
    new StorageObject("PC", kRegSize),
};

long immVals[16] = {
    0         , 1,
    0xFFFFFF00, 0x1F,
    0x0000FFFF, 0x00FFFFFF,
    0x0C      , 0x03,
    0x10      , 0xFF,
    0x0F      , 0x08,
    0x10      , 0x04,
    0x0D      , 0x18
};

// Immediate Registers
StorageObject * IMM[16] = {
    new StorageObject("Imm0", kRegSize, immVals[0x0]),
    new StorageObject("Imm1", kRegSize, immVals[0x1]),
    new StorageObject("Imm2", kRegSize, immVals[0x2]),
    new StorageObject("Imm3", kRegSize, immVals[0x3]),
    new StorageObject("Imm4", kRegSize, immVals[0x4]),
    new StorageObject("Imm5", kRegSize, immVals[0x5]),
    new StorageObject("Imm6", kRegSize, immVals[0x6]),
    new StorageObject("Imm7", kRegSize, immVals[0x7]),
    new StorageObject("Imm8", kRegSize, immVals[0x8]),
    new StorageObject("Imm9", kRegSize, immVals[0x9]),
    new StorageObject("ImmA", kRegSize, immVals[0xA]),
    new StorageObject("ImmB", kRegSize, immVals[0xB]),
    new StorageObject("ImmC", kRegSize, immVals[0xC]),
    new StorageObject("ImmD", kRegSize, immVals[0xD]),
    new StorageObject("ImmE", kRegSize, immVals[0xE]),
    new StorageObject("ImmF", kRegSize, immVals[0xF]),
};

Memory m("Main_Memory", kAddrBusSize, kDataBusSize, kMaxAddr, 1, true);
Memory control_storage("Control_Storage", CU_DATA_SIZE, CU_DATA_SIZE, kMaxAddr, 
                        1, true);

// System Registers
StorageObject * SYS[24] = {
    new StorageObject("uPC", kRegSize),
    new StorageObject("uRET", kRegSize),
    new StorageObject("uR0", kRegSize),
    new StorageObject("uR1", kRegSize),
    new StorageObject("uR2", kRegSize),
    new StorageObject("uCnt", kRegSize),
    new StorageObject("MDR", kRegSize),
    &m.MAR(),
    new StorageObject("OP1Val", kRegSize),
    new StorageObject("OP1Type", kRegSize),
    new StorageObject("OP2Val", kRegSize),
    new StorageObject("OP2Type", kRegSize),
    new StorageObject("OP3Val", kRegSize),
    new StorageObject("OP3Type", kRegSize),
    new StorageObject("OP4Val", kRegSize),
    new StorageObject("OP4Type", kRegSize),
    new StorageObject("OP1Scale", kRegSize),
    new StorageObject("OP2Scale", kRegSize),
    new StorageObject("OP3Scale", kRegSize),
    new StorageObject("OP4Scale", kRegSize),
    new StorageObject("IR", kDataBusSize),
    new StorageObject("uTmp", kRegSize),
    new StorageObject("uRet2", kRegSize),
    new StorageObject("uTmp2", kRegSize),
};

StorageObject uIR("uIR", CU_DATA_SIZE);

static bool uTrace = false;

//==========================<Helper Fxn Declaration>==========================//
// Register locator helpers
StorageObject * snagReg(long regNum);
bool checkImmRegRef(StorageObject * rs, long rT, string operation);

// Trace print helpers 
void print3OpmicroInstr(StorageObject * d, StorageObject * r, StorageObject * t,
                            string operation);
void print2OpmicroInstr(StorageObject *d, StorageObject * r, string operation);
void printCONDmicroInstr(StorageObject * rs, long imm, string operation);
void printMAXOpsMicroInstr(int max);

void macroTraceDecode();
void macroTraceBranch();
void macroTraceFetch();
void macroTraceWriteback();

void setAfield(BusALU * alu);
void setBfield(long Bfield, BusALU * alu);


bool Conditional();
int getMaxOperands(long opc);

//===============================<Primary Fxns>===============================//
void connect() {
    // Memory Setup
    m.MAR().connectsTo(ALU1.OUT());
    m.MAR().connectsTo(ALU2.OUT());	
    control_storage.MAR().connectsTo(ALU1.OUT());

    // Connect PC to main memory
    GPR[15]->connectsTo(m.READ());

    // MDR setup
    SYS[MDR]->connectsTo(ALU1.OUT());
    SYS[MDR]->connectsTo(ALU2.OUT());
    SYS[MDR]->connectsTo(m.WRITE());
    SYS[MDR]->connectsTo(m.READ());
    SYS[MDR]->connectsTo(ALU1.OP1());
    SYS[MDR]->connectsTo(ALU1.OP2());
    SYS[MDR]->connectsTo(ALU2.OP1());
    SYS[MDR]->connectsTo(ALU2.OP2());

    //uPC setup
    SYS[uPC]->connectsTo(ALU1.OUT());
    SYS[uPC]->connectsTo(ALU2.OUT());
    SYS[uPC]->connectsTo(ALU1.OP1());
    SYS[uPC]->connectsTo(ALU2.OP1());
    SYS[uPC]->connectsTo(control_storage.READ());

    // IR setup
    SYS[IR]->connectsTo(ALU1.OUT());
    SYS[IR]->connectsTo(ALU2.OUT());

    //uIR setup
    uIR.connectsTo(control_storage.READ());
    uIR.connectsTo(ALU1.OP1());
    uIR.connectsTo(ALU1.IN1());

    // Connect GPRs and IMMs
    for(int i = 0; i < 16; i++) {
        GPR[i]->connectsTo(ALU1.OUT());
        GPR[i]->connectsTo(ALU2.OUT());
        GPR[i]->connectsTo(ALU1.OP1());
        GPR[i]->connectsTo(ALU1.OP2());
        GPR[i]->connectsTo(ALU2.OP1());
        GPR[i]->connectsTo(ALU2.OP2());

        IMM[i]->connectsTo(ALU1.OP1());
        IMM[i]->connectsTo(ALU1.OP2());
        IMM[i]->connectsTo(ALU2.OP1());
        IMM[i]->connectsTo(ALU2.OP2());
    }

    // Basic connect for Systems Registers to ALU
    for(int i = 0; i < 24; i++) {
        if (i == MDR || i == IR || i == MAR || i == uPC ) {
            continue;
        }
        SYS[i]->connectsTo(ALU1.OUT());
        SYS[i]->connectsTo(ALU2.OUT());
        SYS[i]->connectsTo(ALU1.OP1());
        SYS[i]->connectsTo(ALU1.OP2());
        SYS[i]->connectsTo(ALU2.OP1());
        SYS[i]->connectsTo(ALU2.OP2());
    }

}

void execute(const char * codeFile, const char * uCodeFile, bool doUTrace) {
    uTrace = doUTrace;
    
    // Load the object files
    m.load(codeFile);
    control_storage.load(uCodeFile);

    // Set starting PC Values
    GPR[15]->latchFrom(m.READ());
    SYS[uPC]->latchFrom(control_storage.READ());
    Clock::tick();
    
    long lastuPC = 0;

    cout << "Entering Main Execute Loop\n";
    while(true) { 
        // Check if we were at 0xFFFFFFFF
        if(SYS[uPC]->value() == 0xFFFFFFFF) {
            cout << "HLT\n";
            break;
        }
        
        // Increment Program Counter and store in uPC and MAR
        ALU1.OP1().pullFrom(*SYS[uPC]);
        ALU1.OP2().pullFrom(*IMM[1]);
        ALU1.perform(BusALU::op_add); // Add uPC + 1
        control_storage.MAR().latchFrom(ALU1.OUT());
        SYS[uPC]->latchFrom(ALU1.OUT());
        Clock::tick();
        
        // Trigger macro trace calls here
        switch(SYS[uPC]->value()) {
            case 0x001:
                macroTraceFetch();
                break;
            case 0x200:
                macroTraceDecode();
                break;
            case 0x296:
                macroTraceBranch();
                break;
            case 0x400:
                macroTraceWriteback();
                break;
            default:
                break;
        }

        // Read Value from CS into uIR
        control_storage.read();
        uIR.latchFrom(control_storage.READ()); 
        Clock::tick();
    
        // Can Print uPC, uIR
        if (uTrace) {
            cout << "\t" << setw(3) << setfill('0') << SYS[uPC]->value() << ": " ;
        }
        

        // Execute
        long prefix = (uIR.value() >> 53);
        long bBits;
        
        // Get prefix (first 2 bits of uIR)
        switch(prefix) {
            case 0: // Parallel Operations
                bBits = CU_DATA_SIZE-24;
                setAfield(&ALU1);
                if(uTrace) {
                    cout << ", ";
                }
                setBfield(bBits, &ALU2);
                if(uTrace) {
                    cout << "\n";
                }
                break;
            
            case 1: // A Field jump
                if (uTrace) cout << "uJMP: uPC <- uIR[" 
                                 << (uIR.value() & 0xFFFFFFFF) << "],";
                setAfield(&ALU2);
                if(uTrace) cout << "\n";
                ALU1.IN1().pullFrom(uIR);
                SYS[uPC]->latchFrom(ALU1.OUT());
                ALU1.perform(BusALU::op_rop1);
                break;
            
            case 2: // B Field jump
                bBits = CU_DATA_SIZE-3;
                if (uTrace) cout << "uJMP: uPC <- uIR[" 
                                 << (uIR.value() & 0xFFFFFFFF) << "], ";
                setBfield(bBits, &ALU2);
                if (uTrace) cout << "\n";
                ALU1.IN1().pullFrom(uIR);
                SYS[uPC]->latchFrom(ALU1.OUT());
                ALU1.perform(BusALU::op_rop1);
                break;

            case 3: // Conditional 
                if(Conditional()) {
                    ALU1.IN1().pullFrom(uIR);
                    SYS[uPC]->latchFrom(ALU1.OUT());
                    ALU1.perform(BusALU::op_rop1);
                    if(uTrace) cout << ": TAKEN: uPC <- uIR[" 
                                    << (uIR.value() & 0xFFFFFFFF) << "]\n";
                } else {
                    if(uTrace) cout << ": NOT TAKEN\n";
                }
                break;

            default:
                cout << "uInst Prefix not recognized!\n";
                break;
        }
        Clock::tick(); // Tick our clock

        // Insert an extra line after uJumps
        if (SYS[uPC]->value() - lastuPC != 1 && uTrace) {
            cout << "\n";
        }
        lastuPC = SYS[uPC]->value();
    }

}

//==========================<Helper Fxn Definition>===========================//
StorageObject * snagReg(long regNum) {
    // Get register from number
    if(regNum <= 0x0f) {
        return IMM[regNum];
    } else if (regNum <= 0x2f) {
        return SYS[regNum - 16];
    } else if (regNum <= 0x3f) {
        return GPR[regNum - 48];
    } else {
        cout << "**ERROR** Attempted to access an unknown register '"<< regNum <<"'\n";
        return NULL;
    }
}

int getMaxOperands(long opc) {
    int maxOps;
    switch(opc) {
        case 0x2: // CLR
        case 0x3: // DMP
        case 0x19: // INC
        case 0x1A: // DEC
        case 0x31: // JMP
        case 0x32: // JAL
        case 0x3a: // BAO
        case 0x3b: // BAC
            maxOps = 1;
            break;
        case 0x4: // OUT
        case 0x18: // CMP
        case 0x20: // MOV
        case 0x21: // SWP
            maxOps = 2;
            break;
        case 0x10: // ADD
        case 0x11: // SUB
        case 0x14: // MOD
        case 0x15: // AND
        case 0x16: // OR
        case 0x17: // XOR
        case 0x34: // BEQ
        case 0x35: // BNE
        case 0x36: // BGT
        case 0x37: // BGE
        case 0x38: // BIE
        case 0x39: // BDE
            maxOps = 3;
            break;
        case 0x12: // MUL
        case 0x22: // CPY
        case 0x13: // DIV
            maxOps = 4;
            break;
        default:   // All other instructions
            maxOps = 0;
            break;
    }
    return maxOps;
}

void print3OpmicroInstr(StorageObject * d, StorageObject * r, StorageObject * t, 
                            string operation, bool immR = false, 
                            bool immT = false) {
    cout << d->name() << " <- ";
    if(immR) {
        cout << "0x" << r->value();
    } else {
         cout << r->name() << "[" << r->value() << "]";
    }
    cout << " " << operation << " ";
    if(immT) {
        cout << "0x" << t->value();
    } else {
         cout << t->name() << "[" << t->value() << "]";
    }
}

void print2OpmicroInstr(StorageObject *d, StorageObject * r, string operation,
                            bool immR = false) {
    cout << d->name() << " <- " << operation;
    if(immR) {
        cout << "0x" << r->value();
    } else {
         cout << r->name() << "[" << r->value() << "]";
    }
}

void setAfield(BusALU * alu) {

    long opc = uIR.value() >> 50 & 0x7;
    long rdA = uIR.value() >> 44 & 0x3f;
    long rsA = uIR.value() >> 38 & 0x3f;
    long rtA = uIR.value() >> 32 & 0x3f;

    // Setup Operand register pointers
    StorageObject * dA = snagReg(rdA);
    StorageObject * sA = snagReg(rsA);
    StorageObject * tA = snagReg(rtA);
    bool rsImm = (rsA >= 0 && rsA < 16);
    bool rtImm = (rtA >= 0 && rtA < 16);

    switch(opc) {
        case 0: // No Op
            if (uTrace) cout << "NOP";
            break;

        case 1: // MOV
            if (uTrace) print2OpmicroInstr(dA, sA, "", rsImm);
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_rop1);
            break;

        case 2: // SLL
            if (uTrace) print3OpmicroInstr(dA, sA, tA, "<<", rsImm, rtImm);
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_lshift);
            break;

        case 3: // SRL
            if (uTrace) print3OpmicroInstr(dA, sA, tA, ">>", rsImm, rtImm);
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_rshift);
            break;

        case 4: // OR
            if (uTrace) print3OpmicroInstr(dA, sA, tA, "|", rsImm, rtImm);
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_or);
            break;

        case 5: // SIGN EXTEND
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_extendSign);
            break;

        case 6: // XOR
            if (uTrace) print3OpmicroInstr(dA, sA, tA, "^", rsImm, rtImm);
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_xor);
            break;

        case 7: // AND
            if (uTrace) print3OpmicroInstr(dA, sA, tA, "&", rsImm, rtImm);
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_and);
            break;

        default:
            cout << "**ERROR** Unknown A Field op '"<< opc <<"'\n";
            break;
    }
}

void setBfield(long Bfield, BusALU * alu) {

    long opc, rdB, rsB, rtB;
    bool rsImm = (rsB >= 0 && rsB < 16);
    bool rtImm = (rtB >= 0 && rtB < 16);
    if (Bfield == 52) { // We are looking at the upper half (used in goto B)
        opc = uIR.value() >> 50 & 0x7;
        rdB = uIR.value() >> 44 & 0x3f;
        rsB = uIR.value() >> 38 & 0x3f;
        rtB = uIR.value() >> 32 & 0x3f;
    } else {
        opc = uIR.value() >> 29 & 0x7;
        rdB = uIR.value() >> 23 & 0x3f;
        rsB = uIR.value() >> 17 & 0x3f;
        rtB = uIR.value() >> 11 & 0x3f;
    }

    // Set up operand pointers
    StorageObject * dB = snagReg(rdB);
    StorageObject * sB = snagReg(rsB);
    StorageObject * tB = snagReg(rtB);

    switch(opc) {
        case 0: // No Op
            if (uTrace) cout << "NOP";
            break;

        case 1: // MOV
            if (uTrace) print2OpmicroInstr(dB, sB, "", rsImm);
            alu->OP1().pullFrom(*sB);
            alu->OP2().pullFrom(*tB);
            dB->latchFrom(alu->OUT());
            alu->perform(BusALU::op_rop1);
            break;

        case 2: // CMP
            if (uTrace) print2OpmicroInstr(dB, sB, "~", rsImm);
            alu->OP1().pullFrom(*sB);
            alu->OP2().pullFrom(*tB);
            dB->latchFrom(alu->OUT());
            alu->perform(BusALU::op_not);
            break;

        case 3: // Mem Read
            if (uTrace) cout << "MDR <- MEM[" << SYS[MAR]->value() << "]";
            m.read();
            SYS[MDR]->latchFrom(m.READ());
            break;
            
        case 4: // Mem Write
            if (uTrace) cout << "MEM[" << SYS[MAR]->value() << "] <- MDR";
            m.WRITE().pullFrom(*SYS[MDR]);
            m.write();
            break;

        case 5: // SUB
            if (uTrace) print3OpmicroInstr(dB, sB, tB, "-", rsImm, rtImm);
            alu->OP1().pullFrom(*sB);
            alu->OP2().pullFrom(*tB);
            dB->latchFrom(alu->OUT());
            alu->perform(BusALU::op_sub);
            break;

        case 6: // ADD
            if (uTrace) print3OpmicroInstr(dB, sB, tB, "+", rsImm, rtImm);
            alu->OP1().pullFrom(*sB);
            alu->OP2().pullFrom(*tB);
            dB->latchFrom(alu->OUT());
            alu->perform(BusALU::op_add);
            break;

        case 7: // No Op (undefined)
            if (uTrace) cout << "NOP";
            break;

        default:
            if (uTrace) cout << "**ERROR** unknown B field op '" << opc<< "`\n";
            break;
    }
}

bool checkImmRegRef(StorageObject * rs, long rT, string operation) {
    if(rT < 0 || rT >= 16) {
        cout << "**ERROR** Unable to find immediate register '" << rT <<"'\n";
        return false; 
    }
    if (uTrace) cout << "if " << rs->name() << "[" << rs->value() << "]";
    if (uTrace) cout << operation << immVals[rT];
    return rs->value() == immVals[rT];
}

void printCONDmicroInstr(StorageObject * rs, long imm, string operation) {
    cout << "if " << rs->name() << "[" << rs->value() << "]" << operation 
        << imm;
}

void printMAXOpsMicroInstr(int max) {
    cout << "if IR[" << SYS[IR]->value() << "] MaxNrOps == " << max;
}

bool Conditional() {
    // Note: rT here either equals an 8 bit immediate or an 8 bit reference to 
    //       an IMM register. Because of this Case 2-7 will just compare between
    //       rs->value() and rT directly. If necessary we can implement this in 
    //       a Constant style switch statement from 0-255. We are also doing 
    //       simple bit twiddling with rs as well. This is done because we  
    //       believe that this would be accomplished with simple wire truncation
    //       and standalone gates that wouldn't require a full ALU in real 
    //       hardware.
    long rT = (uIR.value() >> 44) & 0xff;
    long rS = (uIR.value() >> 38) & 0x3f;
    long type = (uIR.value() >> 32) & 0x0f;
    StorageObject * rs = snagReg(rS);

    switch(type) {
        case 0: // Reg Equal
            return checkImmRegRef(rs, rT, " == ");

        case 1: // Reg Not Equal
            return !checkImmRegRef(rs, rT, " != ");

        case 2: // Bits set
            if (uTrace) cout << "if " << rs->name() << "[" << rs->value() << "]" 
                             << " & " << rT << " == " << rT;
            return !((rs->value() & rT) ^ rT);

        case 3: // Bits not set
            if (uTrace) cout << "if " << rs->name() << "[" << rs->value() << "]" 
                             << " & " << rT << " != " << rT;
            return (rs->value() & rT) ^ rT;

        case 4: // Byte equal
            if (uTrace) printCONDmicroInstr(rs, rT, " == ");
            return rs->value() == rT;

        case 5: // Byte Not equal
            if (uTrace) printCONDmicroInstr(rs, rT, " != ");
            return rs->value() != rT;

        case 6: // Nybble 1 equal
            if (uTrace) printCONDmicroInstr(rs, rT, " & 0xf0 == ");
            return !((rs->value() & 0xf0) ^ rT);
            break;

        case 7: // Nybble 1 not equal
            if (uTrace) printCONDmicroInstr(rs, rT, " & 0xf0 != ");
            return (rs->value() & 0xf0) ^ rT;
            break;

        case 8: // IR 0 Ops
            if (uTrace) printMAXOpsMicroInstr(0);
            return getMaxOperands(SYS[IR]->value()) == 0;

        case 9: // IR 1 Op
            if (uTrace) printMAXOpsMicroInstr(1);
            return getMaxOperands(SYS[IR]->value()) == 1;

        case 10: // IR 2 Ops
            if (uTrace) printMAXOpsMicroInstr(2);
            return getMaxOperands(SYS[IR]->value()) == 2;

        case 11: // IR 3 Ops
            if (uTrace) printMAXOpsMicroInstr(3);
            return getMaxOperands(SYS[IR]->value()) == 3;

        case 12: // PC Max
            if (uTrace) cout << "if PC[" << GPR[15]->value() << "] == PC_MAX";
            return GPR[15]->value() >= kMaxAddr;

        default:
            break;
    }
    return false;
}

void printMacroInst(int opc, bool skipMnemonic, long** vals, long * RETS, 
                    long * OP1, long * OP2, long * OP3, long * OP4);

StorageObject* traceSnagReg(long * op) {
    
    if(!((op[0] & 0x0C) ^ 0x00)) 
        return snagReg(0x30);
    if(!((op[0] & 0x0C) ^ 0x04)) 
        return snagReg(0x31);
    if(!((op[0] & 0x0C) ^ 0x08)) 
        return snagReg(0x3D);
    return snagReg(0x30 | (op[1] & 0x0F));
}

#define MACRO_ARG_WIDTH 27
void printMacroArgTxt(long * op) {
    char oBuf[128];
    
    if(op == NULL) {
        for(int i = 0; i <= MACRO_ARG_WIDTH; i++) {
            cout << " ";
        }
        return;
    }

    int opType = op[0] & 0xFF;
    long prefixes = op[0] >> 8;
    bool scaledArg = op[2] != 0;
    bool memArg = !((opType & 0xF0) ^ 0x40) || !((opType & 0xF0) ^ 0x50) ||
                  !((opType & 0xF0) ^ 0x60) || !((opType & 0xF0) ^ 0x70) ||
                  !((opType & 0xF0) ^ 0x80);
    bool didVal = false;
    
    if(!((opType & 0xF0) ^ 0x10)) {             // Nybble Immediate
        sprintf(oBuf, "<IMM> $%08lx", op[0] & 0x0F);
    } else if (!((opType & 0xF0) ^ 0x20)) {     // Immediate
        sprintf(oBuf, "<IMM> $%08lx", op[1]);
    } else if (!((opType & 0xF0) ^ 0x30)) {     // Register
        sprintf(oBuf, "<REG> %%%2s", traceSnagReg(op)->name());
    } else if (!((opType & 0xF0) ^ 0x40)) {     // Register Indirect
        sprintf(oBuf, "<RID> (%%%2s)", traceSnagReg(op)->name());
    } else if (!((opType & 0xF0) ^ 0x50)) {     // Memory Indirect
        sprintf(oBuf, "<MID> @(%08lx)", op[1]);
        
    } else if (!((opType & 0xF0) ^ 0x60)) {     // Index
        StorageObject *r1, *r2;
        int ext1 = (op[1] >> 8) & 0xFF;
        int ext2 = op[1] & 0xFF;
        
        int rs1 = (op[0] >> 2) & 0x03;
        int rs2 = op[0] & 0x03;
        
        //Grab register 1
        if(!(rs1 ^ 0x00)) {
            r1 = snagReg(0x30);
        } else if(!(rs1 ^ 0x00)) {
            r1 = snagReg(0x31);
        } else if(!(rs1 ^ 0x00)) {
            r1 = snagReg(0x3D);
        } else {
            r1 = snagReg(0x30 | (ext1 & 0x0F));
        }
        
        //Grab register 2
        if(!(rs2 ^ 0x00)) {
            r2 = snagReg(0x30);
        } else if(!(rs2 ^ 0x00)) {
            r2 = snagReg(0x31);
        } else if(!(rs2 ^ 0x00)) {
            r2 = snagReg(0x3D);
        } else {
            r2 = snagReg(0x30 | (ext1 & 0x0F));
        }
        
        sprintf(oBuf, "<IDX> (%%%2s+%%%2s)", r1->name(), r2->name());
        
    } else if (!((opType & 0xF0) ^ 0x70)) {     // Displacement
        StorageObject *r1;
        int ext1 = (op[0] >> 24) & 0xFF;
        
        int rs1 = (op[0] >> 2) & 0x03;
        
        //Grab register 1
        if(!(rs1 ^ 0x00)) {
            r1 = snagReg(0x30);
        } else if(!(rs1 ^ 0x00)) {
            r1 = snagReg(0x31);
        } else if(!(rs1 ^ 0x00)) {
            r1 = snagReg(0x3D);
        } else {
            r1 = snagReg(0x30 | (ext1 & 0x0F));
        }
        sprintf(oBuf, "<DSP> %08lx(%%%2s) ", op[1], r1->name()); //todo second reg
    } else if (!((opType & 0xF0) ^ 0x80)) {     // Absolute value
        sprintf(oBuf, "<ABS> %08lx", op[1]);
    } else if (!((opType & 0xF0) ^ 0x90)) {     // Increment
        sprintf(oBuf, "<INC> (%%%2s)+", traceSnagReg(op)->name());
    } else if (!((opType & 0xF0) ^ 0xA0)) {
        sprintf(oBuf, "<DEC> -(%%%2s)", traceSnagReg(op)->name());
    } else if (!((opType & 0xF0) ^ 0xC0)) {
        sprintf(oBuf, "<PIC> +(%%%2s)", traceSnagReg(op)->name());
    } else if (!((opType & 0xF0) ^ 0xD0)) {
        sprintf(oBuf, "<PDC> (%%%2s)-", traceSnagReg(op)->name());
    } else {
        sprintf(oBuf, "(%08lx, %08lx)", op[0], op[1]);
    }
    
    // Scaling
    if(scaledArg) {
        sprintf(oBuf + strlen(oBuf), "[%%%2s,%1lx]", "  ", ((op[2] >> 8) & 0x03) + 1);
    }
    
    // Print out
    cout << oBuf;
    for(int i = strlen(oBuf); i <= MACRO_ARG_WIDTH; i++) {
        cout << " ";
    }
    
}

void printMacroArgVals(long val, long ret = 0, bool doRet = false) {
    char buf[128];
    buf[0] = 0;
    
    if(doRet) {
        sprintf(buf + strlen(buf), "[%08lx] ", ret);
    }
    
    sprintf(buf + strlen(buf), "(%08lx) ", val);
    
    // Print out
    cout << buf;
    for(int i = strlen(buf); i <= MACRO_ARG_WIDTH; i++) {
        cout << " ";
    }
}

void macroTrace(int phase) {
    static int lastPC = 0;
    static int opc = 0x100; //Can't get this otherwise
    static long OP1[3] = {0, 0, 0}, val1 = 0;
    static long OP2[3] = {0, 0, 0}, val2 = 0;
    static long OP3[3] = {0, 0, 0}, val3 = 0;
    static long OP4[3] = {0, 0, 0}, val4 = 0;
    static long * vals[4] = {&val1, &val2, &val3, &val4};
    static long RETS[3] = {0, 0, 0};

    switch(phase) {
        case 0:                                 // Store decode phase values
            opc = SYS[IR]->value();
            
            OP1[0] = SYS[OP1Type]->value();
            OP1[1] = SYS[OP1Val]->value(); 
            OP1[2] = SYS[OP1Scale]->value();
            
            OP2[0] = SYS[OP2Type]->value();
            OP2[1] = SYS[OP2Val]->value(); 
            OP2[2] = SYS[OP2Scale]->value();
            
            OP3[0] = SYS[OP3Type]->value();
            OP3[1] = SYS[OP3Val]->value(); 
            OP3[2] = SYS[OP3Scale]->value();
            
            OP4[0] = SYS[OP4Type]->value();
            OP4[1] = SYS[OP4Val]->value(); 
            OP4[2] = SYS[OP4Scale]->value();
            return;
        case 1:                                 // Store branch phase values
            val1 = SYS[OP1Val]->value();
            val2 = SYS[OP2Val]->value();
            val3 = SYS[OP3Val]->value();
            val4 = SYS[OP4Val]->value();
            return;
        
        case 3:                                 // Store handler returned values
            RETS[0] = SYS[uR0]->value();
            RETS[1] = SYS[uR1]->value();
            RETS[2] = SYS[uR2]->value();
            return;

        case 2:                                 // Print the macro trace
            if (opc == 0x100) {
                lastPC = GPR[15]->value();
                return;
            }
            cout << std::setfill('0') << std::setw(4) << (lastPC + 1) << ": ";
            lastPC = GPR[15]->value();

            printMacroInst(opc, false, vals, RETS, OP1, OP2, OP3, OP4);
            
        default:
            return;
    }
}

void macroTraceDecode() {
    macroTrace(0);
}

void macroTraceBranch() {
    macroTrace(1);
}

void macroTraceFetch() {
    macroTrace(2);
}

void macroTraceWriteback() {
    macroTrace(3);
}

void printMacroInst(int opc, bool skipMnemonic, long** vals, long * RETS, 
                    long * OP1, long * OP2, long * OP3, long * OP4) {
    
            
    switch(opc) {
        // CPU Control instructions
        case 0x00:
            if(!skipMnemonic) cout << "HLT";
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            break;
            
        case 0x02:
            if(!skipMnemonic) cout << "CLR";
            cout << " | ";
            printMacroArgTxt(OP1);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            
            cout << "\n          | ";
            printMacroArgVals(0);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            break;
        case 0x03:
            if(!skipMnemonic) cout << "DMP";
            cout << " | ";
            printMacroArgTxt(OP1);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            
            cout << "\n          | ";
            printMacroArgVals(*vals[0], RETS[0], true);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            break;
        case 0x04:
            if(!skipMnemonic) cout << "OUT";
            cout << " | ";
            printMacroArgTxt(OP1);
            cout << " | ";
            OP2[0] = 0x3F;
            OP2[1] = 0x28;
            OP2[2] = 0x00;
            printMacroArgTxt(OP2);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            
            
            cout << "\n          | ";
            printMacroArgVals(*vals[0]);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            break;
        
        // ALU instructions
        case 0x10:
            if(!skipMnemonic) cout << "ADD";
            cout << " | ";
            printMacroArgTxt(OP1);
            cout << " | ";
            printMacroArgTxt(OP2);
            cout << " | ";
            printMacroArgTxt(OP3);
            cout << " | ";
            printMacroArgTxt(NULL);
            
            cout << "\n          | ";
            printMacroArgVals(*vals[0], RETS[0], true);
            cout << " | ";
            printMacroArgVals(*vals[1]);
            cout << " | ";
            printMacroArgVals(*vals[2]);
            cout << " | ";
            printMacroArgTxt(NULL);
            
            break;
        case 0x11:
            if(!skipMnemonic) cout << "SUB";
            return printMacroInst(0x10, true, vals, RETS, OP1, OP2, OP3, OP4);
        case 0x12:
            if(!skipMnemonic) cout << "MUL";
            cout << " | ";
            printMacroArgTxt(OP1);
            cout << " | ";
            printMacroArgTxt(OP2);
            cout << " | ";
            printMacroArgTxt(OP3);
            cout << " | ";
            printMacroArgTxt(OP4);
            
            cout << "\n          | ";
            printMacroArgVals(*vals[0], RETS[0], true);
            cout << " | ";
            printMacroArgVals(*vals[1], RETS[1], true);
            cout << " | ";
            printMacroArgVals(*vals[2]);
            cout << " | ";
            printMacroArgVals(*vals[3]);
            break;
        case 0x13:
            if(!skipMnemonic) cout << "DIV";
            return printMacroInst(0x12, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x14:
            if(!skipMnemonic) cout << "MOD";
            return printMacroInst(0x10, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x15:
            if(!skipMnemonic) cout << "AND";
            return printMacroInst(0x10, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x16:
            if(!skipMnemonic) cout << " OR";
            return printMacroInst(0x10, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x17:
            if(!skipMnemonic) cout << "XOR";
            return printMacroInst(0x10, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x18:
            if(!skipMnemonic) cout << "CMP";
            cout << " | ";
            printMacroArgTxt(OP1);
            cout << " | ";
            printMacroArgTxt(OP2);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            
            cout << "\n          | ";
            printMacroArgVals(*vals[0], RETS[0], true);
            cout << " | ";
            printMacroArgVals(*vals[1]);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            break;
        case 0x19:
            if(!skipMnemonic) cout << "INC";
            cout << " | ";
            printMacroArgTxt(OP1);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            
            cout << "\n          | ";
            printMacroArgVals(*vals[0], RETS[0], true);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            break;
        case 0x1A:
            if(!skipMnemonic) cout << "DEC";
            return printMacroInst(0x19, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
            
        // Memory Instructions
        case 0x20:
            if(!skipMnemonic) cout << "MOV";
            return printMacroInst(0x18, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x21:
            if(!skipMnemonic) cout << "SWP";
            cout << " | ";
            printMacroArgTxt(OP1);
            cout << " | ";
            printMacroArgTxt(OP2);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            
            cout << "\n          | ";
            printMacroArgVals(*vals[0], RETS[0], true);
            cout << " | ";
            printMacroArgVals(*vals[1], RETS[1], true);
            cout << " | ";
            printMacroArgTxt(NULL);
            cout << " | ";
            printMacroArgTxt(NULL);
            break;
        case 0x22:
            if(!skipMnemonic) cout << "CPY";
            cout << " | ";
            printMacroArgTxt(OP1);
            cout << " | ";
            printMacroArgTxt(OP2);
            cout << " | ";
            printMacroArgTxt(OP3);
            cout << " | ";
            printMacroArgTxt(OP4);
            
            cout << "\n          | ";
            printMacroArgVals(*vals[0]);
            cout << " | ";
            printMacroArgVals(*vals[1]);
            cout << " | ";
            printMacroArgVals(*vals[2]);
            cout << " | ";
            printMacroArgVals(*vals[3]);
            break;
            
        // COL Instructions
        case 0x31:
            if(!skipMnemonic) cout << "JMP";
            return printMacroInst(0x03, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x32:
            if(!skipMnemonic) cout << "JAL";
            return printMacroInst(0x03, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x33:
            if(!skipMnemonic) cout << "RET";
            return printMacroInst(0x00, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x34:
            if(!skipMnemonic) cout << "BEQ";
            cout << " | ";
            printMacroArgTxt(OP1);
            cout << " | ";
            printMacroArgTxt(OP2);
            cout << " | ";
            printMacroArgTxt(OP3);
            cout << " | ";
            printMacroArgTxt(NULL);
            
            cout << "\n          | ";
            printMacroArgVals(*vals[0]);
            cout << " | ";
            printMacroArgVals(*vals[1]);
            cout << " | ";
            printMacroArgVals(*vals[2]);
            cout << " | ";
            printMacroArgTxt(NULL);
            break;
        case 0x35:
            if(!skipMnemonic) cout << "BNE";
            return printMacroInst(0x34, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x36:
            if(!skipMnemonic) cout << "BGT";
            return printMacroInst(0x34, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x37:
            if(!skipMnemonic) cout << "BGE";
            return printMacroInst(0x34, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x38:
            if(!skipMnemonic) cout << "BIE";
            return printMacroInst(0x34, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x39:
            if(!skipMnemonic) cout << "BDE";
            return printMacroInst(0x34, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x3A:
            if(!skipMnemonic) cout << "BAO";
            return printMacroInst(0x03, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        case 0x3B:
            if(!skipMnemonic) cout << "BAC";
            return printMacroInst(0x03, true, vals, RETS, OP1, OP2, OP3, OP4);
            break;
        
        default:
            if(!skipMnemonic) cout << "NOP";
            return printMacroInst(0x00, true, vals, RETS, OP1, OP2, OP3, OP4);
    }
    
    cout << "\n";
    cout << "          | ";
    printMacroArgTxt(NULL);
    cout << " | ";
    printMacroArgTxt(NULL);
    cout << " | ";
    printMacroArgTxt(NULL);
    cout << " |\n";
}
