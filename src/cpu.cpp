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

Memory m("Main Memory", kAddrBusSize, kDataBusSize, kMaxAddr, 1, true);
Memory control_storage("Control Storage", CU_DATA_SIZE, CU_DATA_SIZE, kMaxAddr, 1, true);

// System Registers
StorageObject * SYS[23] = {
    new StorageObject("uPC", kRegSize),
    new StorageObject("uRET", kRegSize),
    new StorageObject("uR0", kRegSize),
    new StorageObject("uR1", kRegSize),
    new StorageObject("uR2", kRegSize),
    new StorageObject("uCnt", kRegSize),
    new StorageObject("MDR", kRegSize),
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
    new StorageObject("uIR", CU_DATA_SIZE),
    new StorageObject("uTmp", kRegSize),
    new StorageObject("uRet2", kRegSize),
};

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
    SYS[uPC]->connectsTo(control_storage.READ());

    // IR setup
    SYS[IR]->connectsTo(ALU1.OUT());
    SYS[IR]->connectsTo(ALU2.OUT());

    //uIR setup
    SYS[uIR]->connectsTo(control_storage.READ());
    SYS[uIR]->connectsTo(ALU1.OP1());
    SYS[uIR]->connectsTo(ALU1.IN1());

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
	if (i == MDR || i == IR || i == uIR || i == uPC ) {
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

void setAfield(long Afield, BusALU * alu) {

    long opc = (*SYS[uIR])(Afield, Afield-2);
    long rdA = (*SYS[uIR])(Afield-3, Afield-8);
    long rsA = (*SYS[uIR])(Afield-9, Afield-14);
    long rtA = (*SYS[uIR])(Afield-15, Afield-20);

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
		break;
	case 1: // MOV
		alu->OP1().pullFrom(*sA);
		alu->OP2().pullFrom(*tA);
		dA->latchFrom(alu->OUT());
		alu->perform(BusALU::op_rop1);
		break;
	case 2: // SLL
		alu->OP1().pullFrom(*sA);
		alu->OP2().pullFrom(*tA);
		dA->latchFrom(alu->OUT());
		alu->perform(BusALU::op_lshift);
		break;
	case 3: // SRL
		alu->OP1().pullFrom(*sA);
		alu->OP2().pullFrom(*tA);
		dA->latchFrom(alu->OUT());
		alu->perform(BusALU::op_rshift);
		break;
	case 4: // OR
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
		alu->OP1().pullFrom(*sA);
		alu->OP2().pullFrom(*tA);
		dA->latchFrom(alu->OUT());
		alu->perform(BusALU::op_xor);
		break;
	case 7: // AND
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

    long opc = (*SYS[uIR])(Bfield, Bfield-2);
    long rdB = (*SYS[uIR])(Bfield-3, Bfield-8);
    long rsB = (*SYS[uIR])(Bfield-9, Bfield-14);
    long rtB = (*SYS[uIR])(Bfield-15 , Bfield-20);

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
		break;
	case 1: // MOV
		alu->OP1().pullFrom(*sB);
		alu->OP2().pullFrom(*tB);
		dB->latchFrom(alu->OUT());
		alu->perform(BusALU::op_rop1);
		break;
	case 2: // CMP
		alu->OP1().pullFrom(*sB);
		alu->OP2().pullFrom(*tB);
		dB->latchFrom(alu->OUT());
		alu->perform(BusALU::op_not);
		break;
	case 3: // Mem Read
		m.read();
		SYS[MDR]->latchFrom(m.READ());
		break;
	case 4: // Mem Write
		m.WRITE().pullFrom(*SYS[MDR]);
		m.write();
		break;
	case 5: // SUB
		alu->OP1().pullFrom(*sB);
		alu->OP2().pullFrom(*tB);
		dB->latchFrom(alu->OUT());
		alu->perform(BusALU::op_sub);
		break;
	case 6: // ADD
		alu->OP1().pullFrom(*sB);
		alu->OP2().pullFrom(*tB);
		dB->latchFrom(alu->OUT());
		alu->perform(BusALU::op_add);
		break;
	case 7: // No Op
		break;
	default:
		cout << "B Field not found!!\n";
		break;
    }
}

bool checkImmRegRef(StorageObject * rs, long rT) {
	switch (rT) {
	    case 0:
		    if (rs->value() == 0) {
		        return true;
		    }
		    break;
	    case 0x01:
		    if (rs->value() == 0x01) {
		        return true;
		    }
		    break;
 	    case 0x02:
		    if (rs->value() == 0xFFFFFF00) {
		        return true;
		    }
		    break;
 	    case 0x03:
		    if (rs->value() == 0x1F) {
		        return true;
		    }
		    break;
 	    case 0x04:
		    if (rs->value() == 0x0000FFFF) {
			return true;
		    }
		    break;
 	    case 0x05:
		    if (rs->value() == 0x00FFFFFF) {
			return true;
		    }
		    break;
 	    case 0x06:
		    if (rs->value() == 0x0C) {
			return true;
		    }
		    break;
 	    case 0x07:
		    if (rs->value() == 0x03) {
			return true;
		    }
		    break;
 	    case 0x08:
		    if (rs->value() == 0x10) {
			return true;
		    }
		    break;
 	    case 0x09:
		    if (rs->value() == 0xFF) {
			return true;
		    }
		    break;
 	    case 0xa:
		    if (rs->value() == 0x0F) {
			return true;
		    }
		    break;
 	    case 0xb:
		    if (rs->value() == 0x08) {
			return true;
		    }
		    break;
 	    case 0xc:
		    if (rs->value() == 0x10) {
			return true;
		    }
		    break;
 	    case 0xd:
		    if (rs->value() == 0x04) {
			return true;
		    }
		    break;
 	    case 0xe:
		    if (rs->value() == 0x0D) {
			return true;
		    }
		    break;
 	    case 0xf:
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

bool Conditional(long cBits) {
    // Note: rT here either equals an 8 bit immediate or an 8 bit reference to an IMM register
    // Because of this Case 2-7 will just compare between rs->value() and rT directly.
    // If necessary we can implement this in a Constant style switch statement from 0-255.
    // We are also doing simple bit twiddling with rs as well. This is done because we 
    // believe that this would be accomplished with simple wire truncation and standard
    // hardware practices that wouldn't require an ALU in real hardware. 
    long rT = (*SYS[uIR])(cBits-1, cBits-8); // 8 bits
    long rS = (*SYS[uIR])(cBits-9, cBits-14);
    long type = (*SYS[uIR])(cBits-17, cBits-20);

    StorageObject * rs;

    // Setup rS pointer
    rs = snagReg(rS);
    long IRopc;

    switch(type) {
        case 0: // Reg Equal
		return checkImmRegRef(rs, rT);
	case 1: // Reg Not Equal
		return !checkImmRegRef(rs, rT);
	case 2: // Bits set
		if(rs->value() & rT == rT) {
		    return true;
		}
		break;
	case 3: // Bits not set
		if(rs->value() & rT != rT) {
		    return true;
		}
		break;
	case 4: // Byte equal
		if(rs->value() == rT) {
		    return true;
		}
		break;
	case 5: // Byte Not equal
		if(rs->value() != rT) {
		    return true;
		}
		break;
	case 6: // Nybble 1 equal
		if(rs->value() & 0xf0 == rT) {
		    return true;
		}
		break;
	case 7: // Nybble 1 not equal
		if(rs->value() & 0xf0 != rT) {
		    return true;
		}
		break;
	case 8: // IR 0 Ops
		IRopc = SYS[IR]->value();
		if(getMaxOperands(IRopc) == 0) {
		    return true;
		}
		break;
	case 9: // IR 1 Op
		IRopc = SYS[IR]->value();
		if(getMaxOperands(IRopc) == 1) {
		    return true;
		}
		break;
	case 10: // IR 2 Ops
		IRopc = SYS[IR]->value();
		if(getMaxOperands(IRopc) == 2) {
		    return true;
		}
		break;
	case 11: // IR 3 Ops
		IRopc = SYS[IR]->value();
		if(getMaxOperands(IRopc) == 3) {
		    return true;
		}
		break;
	case 12: // PC Max
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
    
    cout << "Entering main Execute Loop\n";
    while(true) { 

	// Increment Program Counter and store in uPC and MAR
	ALU1.OP1().pullFrom(*SYS[uPC]);
	ALU1.OP2().pullFrom(*IMM[1]);
	ALU1.perform(BusALU::op_add); // Add uPC + 1
	control_storage.MAR().latchFrom(ALU1.OUT());
	SYS[uPC]->latchFrom(ALU1.OUT());
	Clock::tick();
	
	// Read Value from CS into uIR
	control_storage.read();
	SYS[uIR]->latchFrom(control_storage.READ()); 
	
	// Execute
	long prefix = (*SYS[uIR])(CU_DATA_SIZE-1, CU_DATA_SIZE-2);
	long aBits;
	long bBits;
	long cBits;
	
	// Get prefix (first 2 bits of uIR)
	switch(prefix) {
	    case 0: // Parallel Operations
		aBits = CU_DATA_SIZE-3;
		bBits = CU_DATA_SIZE-24;
		setAfield(aBits, &ALU1);
		setBfield(bBits, &ALU2);
		break;
	    case 1: // A Field jump
		aBits = CU_DATA_SIZE-3;
		setAfield(aBits, &ALU2);
		ALU1.IN1().pullFrom(*SYS[uIR]);
		SYS[uPC]->latchFrom(ALU1.OUT());
		break;
	    case 2: // B Field jump
		bBits = CU_DATA_SIZE-3;
		setBfield(bBits, &ALU2);
		ALU1.IN1().pullFrom(*SYS[uIR]);
		SYS[uPC]->latchFrom(ALU1.OUT());
		break;
	     case 3: // Conditional 
		cBits = CU_DATA_SIZE-3;
	        if(Conditional(cBits)) {
		    ALU1.IN1().pullFrom(*SYS[uIR]);
		    SYS[uPC]->latchFrom(ALU1.OUT());
		}
		break;
	     default:
		cout << "Prefix not found!\n";
		break;
	}
	Clock::tick(); // Tick our clock
    }
}
