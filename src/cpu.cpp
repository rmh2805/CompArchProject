#include "cpu.h"

//=========================================<CPU Objects>==========================================//
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

// Immediate Registers
StorageObject * IMM[16] = {
    new StorageObject("Imm0", kRegSize, 0x0),
    new StorageObject("Imm1", kRegSize, 0x1),
    new StorageObject("Imm2", kRegSize, 0xFFFFFF00),
    new StorageObject("Imm3", kRegSize, 0x1F),
    new StorageObject("Imm4", kRegSize, 0x0000FFFF),
    new StorageObject("Imm5", kRegSize, 0x00FFFFFF),
    new StorageObject("Imm6", kRegSize, 0x0C),
    new StorageObject("Imm7", kRegSize, 0x03),
    new StorageObject("Imm8", kRegSize, 0x10),
    new StorageObject("Imm9", kRegSize, 0xFF),
    new StorageObject("ImmA", kRegSize, 0x0F),
    new StorageObject("ImmB", kRegSize, 0x08),
    new StorageObject("ImmC", kRegSize, 0x10),
    new StorageObject("ImmD", kRegSize, 0x04),
    new StorageObject("ImmE", kRegSize, 0x0D),
    new StorageObject("ImmF", kRegSize, 0x18),
};

Memory m("Main_Memory", kAddrBusSize, kDataBusSize, kMaxAddr, 1, true);
Memory control_storage("Control_Storage", CU_DATA_SIZE, CU_DATA_SIZE, kMaxAddr, 1, true);

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

    // Connect GPRs 
    for(int i = 0; i < 16; i++) {
        GPR[i]->connectsTo(ALU1.OUT());
        GPR[i]->connectsTo(ALU2.OUT());
        GPR[i]->connectsTo(ALU1.OP1());
        GPR[i]->connectsTo(ALU1.OP2());
        GPR[i]->connectsTo(ALU2.OP1());
        GPR[i]->connectsTo(ALU2.OP2());
    }

    // Connect Immediate registers
    for(int i = 0; i < 16; i++) {
        IMM[i]->connectsTo(ALU1.OP1());
        IMM[i]->connectsTo(ALU1.OP2());
        IMM[i]->connectsTo(ALU2.OP1());
        IMM[i]->connectsTo(ALU2.OP2());
    }

    // Basic connect for Systems Registers to ALU
    for(int i = 0; i < 23; i++) {
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

StorageObject * snagReg(long regNum) {
    // Get register from number
    if(regNum <= 0x0f) {
        return IMM[regNum];
    } else if (regNum <= 0x2f) {
        return SYS[regNum - 16];
    } else if (regNum <= 0x3f) {
        return GPR[regNum - 48];
    } else {
        cout << "PANIC!\n";
        return NULL;
    }
}

int getMaxOperands(long opc) {

    int maxOps;
    switch(opc) {
        case 0x0: // HLT
            maxOps = 0;
            break;
        case 0x1: // NOP
            maxOps = 0;
            break;
        case 0x2: // CLR
            maxOps = 1;
            break;
        case 0x3: // DMP
            maxOps = 1;
            break;
        case 0x4: // OUT
            maxOps = 2;
            break;
        case 0x10: // ADD
            maxOps = 3;
            break;
        case 0x11: // SUB
            maxOps = 3;
            break;
        case 0x12: // MUL
            maxOps = 4;
            break;
        case 0x13: // DIV
            maxOps = 4;
            break;
        case 0x14: // MOD
            maxOps = 3;
            break;
        case 0x15: // AND
            maxOps = 3;
            break;
        case 0x16: // OR
            maxOps = 3;
            break;
        case 0x17: // XOR
            maxOps = 3;
            break;
        case 0x18: // CMP
            maxOps = 2;
            break;
        case 0x19: // INC
            maxOps = 1;
            break;
        case 0x1A: // DEC
            maxOps = 1;
            break;
        case 0x20: // MOV
    	    maxOps = 2;
            break;
        case 0x21: // SWP
            maxOps = 2;	    
            break;
        case 0x22: // CPY
            maxOps = 4;    
            break;
        case 0x31: // JMP
            maxOps = 1;
            break;
        case 0x32: // JAL
            maxOps = 1;
            break;
        case 0x33: // RET
            maxOps = 0;
            break;
        case 0x34: // BEQ
            maxOps = 3;
            break;
        case 0x35: // BNE
            maxOps = 3;
            break;
        case 0x36: // BGT
            maxOps = 3;
            break;
        case 0x37: // BGE
            maxOps = 3;
            break;
        case 0x38: // BIE
            maxOps = 3;
            break;
        case 0x39: // BDE
            maxOps = 3;
            break;
        case 0x3a: // BAO
            maxOps = 1;
            break;
        case 0x3b: // BAC
            maxOps = 1;
            break;
        default: 
            cout << "Unspecified Instruction...returning 0 for max OPs\n";
            maxOps = 0;
            break;
    }
    return maxOps;
}

void print3OpmicroInstr(StorageObject * d, StorageObject * r, StorageObject * t, char * operation) {
    cout << d->name() << " <- " << r->name() << "[" << r->value() << "]"
         << " " << operation << " " << t->name() << "[" << t->value() << "]";
}

void print2OpmicroInstr(StorageObject *d, StorageObject * r, char * operation) {
    cout << d->name() << " <- " << operation << r->name() << "[" << r->value() << "]";
}

void setAfield(long Afield, BusALU * alu) {

    long opc = uIR(Afield, Afield-2);
    long rdA = uIR(Afield-3, Afield-8);
    long rsA = uIR(Afield-9, Afield-14);
    long rtA = uIR(Afield-15, Afield-20);

    StorageObject * dA;
    StorageObject * sA;
    StorageObject * tA;

    // Setup rdA pointer
    dA = snagReg(rdA);

    // Setup rsA pointer
    sA = snagReg(rsA);

    // Setup rsA pointer
    tA = snagReg(rtA);

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

    long opc = uIR(Bfield, Bfield-2);
    long rdB = uIR(Bfield-3, Bfield-8);
    long rsB = uIR(Bfield-9, Bfield-14);
    long rtB = uIR(Bfield-15 , Bfield-20);

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

bool checkImmRegRef(StorageObject * rs, long rT, char * operation) {
    cout << "\t" << "if " << rs->name() << "[" << rs->value() << "]" << operation;
	switch (rT) {
        case 0:
            cout << 0;
		    if (rs->value() == 0) {
		        return true;
		    }
		    break;

        case 0x01:
            cout << 1;
		    if (rs->value() == 0x01) {
		        return true;
		    }
		    break;

        case 0x02:
            cout << 0xFFFFFF00;
		    if (rs->value() == 0xFFFFFF00) {
		        return true;
		    }
		    break;

        case 0x03:
            cout << 0x1F;
		    if (rs->value() == 0x1F) {
		        return true;
		    }
		    break;

        case 0x04:
            cout << 0x0000FFFF;
		    if (rs->value() == 0x0000FFFF) {
                return true;
		    }
		    break;

        case 0x05:
            cout << 0x00FFFFFF;
		    if (rs->value() == 0x00FFFFFF) {
                return true;
		    }
		    break;

        case 0x06:
            cout << 0x0C;
		    if (rs->value() == 0x0C) {
                return true;
		    }
		    break;

        case 0x07:
            cout << 0x03;
		    if (rs->value() == 0x03) {
                return true;
		    }
		    break;

        case 0x08:
            cout << 0x10;
		    if (rs->value() == 0x10) {
                return true;
		    }
		    break;

        case 0x09:
            cout << 0xFF;
		    if (rs->value() == 0xFF) {
                return true;
		    }
		    break;

        case 0xa:
            cout << 0x0F;
		    if (rs->value() == 0x0F) {
                return true;
		    }
		    break;

        case 0xb:
            cout << 0x08;
		    if (rs->value() == 0x08) {
                return true;
		    }
		    break;

        case 0xc:
            cout << 0x10;
		    if (rs->value() == 0x10) {
                return true;
		    }
		    break;

        case 0xd:
            cout << 0x04;
		    if (rs->value() == 0x04) {
                return true;
		    }
		    break;

        case 0xe:
            cout << 0x0D;
		    if (rs->value() == 0x0D) {
                return true;
		    }
		    break;

        case 0xf:
            cout << 0x18;
		    if (rs->value() == 0x18) {
                return true;
		    }
		    break;

        default: 
		    cout << "PANIC: We couldn't find an IMM Reg \n";
		    break;

	}
	return false;
}

void printCONDmicroInstr(StorageObject * rs, long imm, char * operation) {
    cout << "if " << rs->name() << "[" << rs->value() << "]" << operation << imm;
}

void printMAXOpsMicroInstr(int max) {
    cout << "if IR[" << SYS[IR]->value() << "] MaxNrOps == " << max;
}

bool Conditional(long cBits) {
    // Note: rT here either equals an 8 bit immediate or an 8 bit reference to an IMM register
    // Because of this Case 2-7 will just compare between rs->value() and rT directly.
    // If necessary we can implement this in a Constant style switch statement from 0-255.
    // We are also doing simple bit twiddling with rs as well. This is done because we 
    // believe that this would be accomplished with simple wire truncation and standard
    // hardware practices that wouldn't require an ALU in real hardware. 
    long rT = uIR(cBits-1, cBits-8); // 8 bits
    long rS = uIR(cBits-9, cBits-14);
    long type = uIR(cBits-17, cBits-20);

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
            if(rs->value() & 0xf0 == rT) {
                return true;
            }
            break;

        case 7: // Nybble 1 not equal
            printCONDmicroInstr(rs, rT, " & 0xf0 != ");
            if(rs->value() & 0xf0 != rT) {
                return true;
            }
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
        cout << "uPC[" << SYS[uPC]->value() << "]: ";
        
        // Execute
        long prefix = uIR(CU_DATA_SIZE-1, CU_DATA_SIZE-2);
        long aBits;
        long bBits;
        long cBits;
        
        // Get prefix (first 2 bits of uIR)
        switch(prefix) {
            case 0: // Parallel Operations
                aBits = CU_DATA_SIZE-3;
                bBits = CU_DATA_SIZE-24;
                setAfield(aBits, &ALU1);
                cout << ", ";
                setBfield(bBits, &ALU2);
                cout << "\n";
                break;
            
            case 1: // A Field jump
                aBits = CU_DATA_SIZE-3;
                setAfield(aBits, &ALU2);
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
                cBits = CU_DATA_SIZE-3;
                if(Conditional(cBits)) {
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
