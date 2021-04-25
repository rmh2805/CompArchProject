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

void execute(const char * codeFile, const char * uCodeFile) {
    // Load the object files
    m.load(codeFile);
    control_storage.load(uCodeFile);

    // Set starting PC Values
    GPR[15]->latchFrom(m.READ());
    SYS[uPC]->latchFrom(control_storage.READ());
    Clock::tick();
    
    cout << "Entering Main Execute Loop\n";
    while(true) { 
        // Check if we were at 0xFFFFFFFF
        if(SYS[uPC]->value() == 0xFFFFFFFF) {
            cout << "HALTING!\n";
            break;
        }
        
        // Increment Program Counter and store in uPC and MAR
        ALU1.OP1().pullFrom(*SYS[uPC]);
        ALU1.OP2().pullFrom(*IMM[1]);
        ALU1.perform(BusALU::op_add); // Add uPC + 1
        control_storage.MAR().latchFrom(ALU1.OUT());
        SYS[uPC]->latchFrom(ALU1.OUT());
        Clock::tick();
        
        // Read Value from CS into uIR
        control_storage.read();
        uIR.latchFrom(control_storage.READ()); 
        Clock::tick();

        // Can Print uPC, uIR
        cout << "uPC[" << SYS[uPC]->value() << "]: " ;
        
        // Execute
        long prefix = (uIR.value() >> 53);
        long bBits;
        
        // Get prefix (first 2 bits of uIR)
        switch(prefix) {
            case 0: // Parallel Operations
                bBits = CU_DATA_SIZE-24;
                setAfield(&ALU1);
                cout << ", ";
                setBfield(bBits, &ALU2);
                cout << "\n";
                break;
            
            case 1: // A Field jump
                setAfield(&ALU2);
                cout << ", ";
                ALU1.IN1().pullFrom(uIR);
                SYS[uPC]->latchFrom(ALU1.OUT());
                ALU1.perform(BusALU::op_rop1);
                cout << "uJMP: uPC <- uIR[" << (uIR.value() & 0xFFFFFFFF) << "]\n";
                break;
            
            case 2: // B Field jump
                bBits = CU_DATA_SIZE-3;
                cout << "uJMP: uPC <- uIR[" << (uIR.value() & 0xFFFFFFFF) << "], ";
                setBfield(bBits, &ALU2);
                cout << "\n";
                ALU1.IN1().pullFrom(uIR);
                SYS[uPC]->latchFrom(ALU1.OUT());
                ALU1.perform(BusALU::op_rop1);
                break;

            case 3: // Conditional 
                if(Conditional()) {
                    ALU1.IN1().pullFrom(uIR);
                    SYS[uPC]->latchFrom(ALU1.OUT());
                    ALU1.perform(BusALU::op_rop1);
                    cout << ": TAKEN: uPC <- uIR[" << (uIR.value() & 0xFFFFFFFF) << "]\n";
                } else {
                    cout << ": NOT TAKEN\n";
                }
                break;

            default:
                cout << "Prefix not found!\n";
                break;
        }
        Clock::tick(); // Tick our clock
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

void print3OpmicroInstr(StorageObject * d, StorageObject * r, StorageObject * t, string operation) {
    cout << d->name() << " <- " << r->name() << "[" << r->value() << "]"
         << " " << operation << " " << t->name() << "[" << t->value() << "]";
}

void print2OpmicroInstr(StorageObject *d, StorageObject * r, string operation) {
    cout << d->name() << " <- " << operation << r->name() << "[" << r->value() << "]";
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

    switch(opc) {
        case 0: // No Op
            cout << "NOP";
            break;
        case 1: // MOV
            print2OpmicroInstr(dA, sA, "");
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_rop1);
            break;
        case 2: // SLL
            print3OpmicroInstr(dA, sA, tA, "<<");
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_lshift);
            break;
        case 3: // SRL
            print3OpmicroInstr(dA, sA, tA, ">>");
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_rshift);
            break;
        case 4: // OR
            print3OpmicroInstr(dA, sA, tA, "|");
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
            print3OpmicroInstr(dA, sA, tA, "^");
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_xor);
            break;
        case 7: // AND
            print3OpmicroInstr(dA, sA, tA, "&");
            alu->OP1().pullFrom(*sA);
            alu->OP2().pullFrom(*tA);
            dA->latchFrom(alu->OUT());
            alu->perform(BusALU::op_and);
            break;
        default:
            cout << "A Field not found!!\n";
            break;
    }
}

void setBfield(long Bfield, BusALU * alu) {

    long opc;
    long rdB;
    long rsB;
    long rtB;
    if (Bfield == 52) { // We are looking at the beginning   
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

    StorageObject * dB;
    StorageObject * sB;
    StorageObject * tB;

    // Setup rdB pointer
    dB = snagReg(rdB);

    // Setup rsB pointer
    sB = snagReg(rsB);

    // Setup rtB pointer
    tB = snagReg(rtB);

    switch(opc) {
        case 0: // No Op
            cout << "NOP";
            break;
        case 1: // MOV
            print2OpmicroInstr(dB, sB, "");
            alu->OP1().pullFrom(*sB);
            alu->OP2().pullFrom(*tB);
            dB->latchFrom(alu->OUT());
            alu->perform(BusALU::op_rop1);
            break;
        case 2: // CMP
            print2OpmicroInstr(dB, sB, "~");
            alu->OP1().pullFrom(*sB);
            alu->OP2().pullFrom(*tB);
            dB->latchFrom(alu->OUT());
            alu->perform(BusALU::op_not);
            break;
        case 3: // Mem Read
            cout << "MDR <- MEM[" << SYS[MAR]->value() << "]";
            m.read();
            SYS[MDR]->latchFrom(m.READ());
            break;
        case 4: // Mem Write
            cout << "MEM[" << SYS[MAR]->value() << "] <- MDR";
            m.WRITE().pullFrom(*SYS[MDR]);
            m.write();
            break;
        case 5: // SUB
            print3OpmicroInstr(dB, sB, tB, "-");
            alu->OP1().pullFrom(*sB);
            alu->OP2().pullFrom(*tB);
            dB->latchFrom(alu->OUT());
            alu->perform(BusALU::op_sub);
            break;
        case 6: // ADD
            print3OpmicroInstr(dB, sB, tB, "+");
            alu->OP1().pullFrom(*sB);
            alu->OP2().pullFrom(*tB);
            dB->latchFrom(alu->OUT());
            alu->perform(BusALU::op_add);
            break;
        case 7: // No Op
            cout << "NOP";
            break;
        default:
            cout << "B Field not found!!\n";
            break;
    }
}

bool checkImmRegRef(StorageObject * rs, long rT, string operation) {
    if(rT < 0 || rT >= 16) {
        cout << "**ERROR** Unable to find immediate register '" << rT <<"'\n";
        return false; 
    }
    cout << "if " << rs->name() << "[" << rs->value() << "]" << operation;
    cout << immVals[rT];
    return rs->value() == immVals[rT];
}

void printCONDmicroInstr(StorageObject * rs, long imm, string operation) {
    cout << "if " << rs->name() << "[" << rs->value() << "]" << operation << imm;
}

void printMAXOpsMicroInstr(int max) {
    cout << "if IR[" << SYS[IR]->value() << "] MaxNrOps == " << max;
}

bool Conditional() {
    // Note: rT here either equals an 8 bit immediate or an 8 bit reference to an IMM register
    // Because of this Case 2-7 will just compare between rs->value() and rT directly.
    // If necessary we can implement this in a Constant style switch statement from 0-255.
    // We are also doing simple bit twiddling with rs as well. This is done because we 
    // believe that this would be accomplished with simple wire truncation and standard
    // hardware practices that wouldn't require an ALU in real hardware. 
    long rT = (uIR.value() >> 44) & 0xff;
    long rS = (uIR.value() >> 38) & 0x3f;
    long type = (uIR.value() >> 32) & 0x0f;
    StorageObject * rs;

    // Setup rS pointer
    rs = snagReg(rS);
    long IRopc;

    switch(type) {
        case 0: // Reg Equal
            return checkImmRegRef(rs, rT, " == ");

        case 1: // Reg Not Equal
            return !checkImmRegRef(rs, rT, " != ");

        case 2: // Bits set
            cout << "if " << rs->name() << "[" << rs->value() << "]" << "& " << rT
                 << " == " << rT;
            if(rs->value() & rT == rT) {
                return true;
            }
            break;

        case 3: // Bits not set
            cout << "if " << rs->name() << "[" << rs->value() << "]" << "& " << rT
                 << " != " << rT;
            if(rs->value() & rT != rT) {
                return true;
            }
            break;

        case 4: // Byte equal
            printCONDmicroInstr(rs, rT, " == ");
            if(rs->value() == rT) {
                return true;
            }
            break;

        case 5: // Byte Not equal
            printCONDmicroInstr(rs, rT, " != ");
            if(rs->value() != rT) {
                return true;
            }
            break;

        case 6: // Nybble 1 equal
            printCONDmicroInstr(rs, rT, " & 0xf0 == ");
            return !((rs->value() & 0xf0) ^ rT);
            break;

        case 7: // Nybble 1 not equal
            printCONDmicroInstr(rs, rT, " & 0xf0 != ");
            return (rs->value() & 0xf0) ^ rT;
            break;

        case 8: // IR 0 Ops
            printMAXOpsMicroInstr(0);
            IRopc = SYS[IR]->value();
            if(getMaxOperands(IRopc) == 0) {
                return true;
            }
            break;

        case 9: // IR 1 Op
            printMAXOpsMicroInstr(1);
            IRopc = SYS[IR]->value();
            if(getMaxOperands(IRopc) == 1) {
                return true;
            }
            break;

        case 10: // IR 2 Ops
            printMAXOpsMicroInstr(2);
            IRopc = SYS[IR]->value();
            if(getMaxOperands(IRopc) == 2) {
                return true;
            }
            break;

        case 11: // IR 3 Ops
            printMAXOpsMicroInstr(3);
            IRopc = SYS[IR]->value();
            if(getMaxOperands(IRopc) == 3) {
                return true;
            }
            break;

        case 12: // PC Max
            printCONDmicroInstr(GPR[15], kMaxAddr, " == ");
            if(GPR[15]->value() == kMaxAddr) {
                return true;
            }
            break;

        default:
            break;
    }
    return false;
}
