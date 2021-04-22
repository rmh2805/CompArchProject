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

//=========================================<uPU Objects>==========================================//
Memory control_storage("Control Storage", kAddrBusSize, kDataBusSize);

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
    new StorageObject("uIR", kRegSize),
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

	    // Read Value from 
	    control_storage.read();
	    SYS[uIR]->latchFrom(control_storage.READ()); 

	    // Execute
	    
    }

}
