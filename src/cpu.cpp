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

Memory m("Main Memory", kAddrBusSize, kDataBusSize);
Memory control_storage("Control Storage", CU_DATA_SIZE, CU_DATA_SIZE);

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
    new StorageObject("IR", kRegSize),
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
    for(int i = 0; i < 16; i++) {
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

void setAfield(long Afield, BusALU * alu) {

    long opc = (*SYS[uIR])(CU_DATA_SIZE-Afield, CU_DATA_SIZE-Afield-2);
    long rdA = (*SYS[uIR])(CU_DATA_SIZE-Afield-3, CU_DATA_SIZE-Afield-8);
    long rsA = (*SYS[uIR])(CU_DATA_SIZE-Afield-9, CU_DATA_SIZE-Afield-14);
    long rtA = (*SYS[uIR])(CU_DATA_SIZE-Afield-15 , CU_DATA_SIZE-Afield-20);

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
		break;
    }
}

void setBfield(long Bfield, BusALU * alu) {

    long opc = (*SYS[uIR])(CU_DATA_SIZE-Bfield, CU_DATA_SIZE-Bfield-2);
    long rdB = (*SYS[uIR])(CU_DATA_SIZE-Bfield-3, CU_DATA_SIZE-Bfield-8);
    long rsB = (*SYS[uIR])(CU_DATA_SIZE-Bfield-9, CU_DATA_SIZE-Bfield-14);
    long rtB = (*SYS[uIR])(CU_DATA_SIZE-Bfield-15 , CU_DATA_SIZE-Bfield-20);

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
    long rT = (*SYS[uIR])(CU_DATA_SIZE-cBits-1, CU_DATA_SIZE-cBits-8); // 8 bits
    long rS = (*SYS[uIR])(CU_DATA_SIZE-cBits-9, CU_DATA_SIZE-cBits-14);
    long type = (*SYS[uIR])(CU_DATA_SIZE-cBits-17, CU_DATA_SIZE-cBits-20);

    StorageObject * rs;
    StorageObject * rt;

    // Setup rS pointer
    rs = snagReg(rS);

    switch(type) {
        case 0: // Reg Equal
		// Switch on RT
		return checkImmRegRef(rs, rT);
	case 1: // Reg Not Equal
		// Switch on RT
		return !checkImmRegRef(rs, rT);
	case 2: // Bits set
		break;
	case 3: // Bits not set
		break;
	case 4: // Byte equal
		break;
	case 5: // Byte Not equal
		break;
	case 6: // Nybble 1 equal
		break;
	case 7: // Nybble 1 not equal
		break;
	case 8:
		break;
	case 9:
		break;
	case 10:
		break;
	case 11:
		break;
	case 12:
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
		break;
	}
	Clock::tick(); // Tick our clock
    }
}
