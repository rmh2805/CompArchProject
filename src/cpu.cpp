#include "cpu.h"

//=========================================<CPU Objects>==========================================//
Bus ALU1Out("ALU1Out", kAddrBusSize);
Bus ALU2Out("ALU2Out", kAddrBusSize);
Bus ALU1A("ALU1A", kAddrBusSize);
Bus ALU1B("ALU1B", kAddrBusSize);
Bus ALU2A("ALU2A", kAddrBusSize);
Bus ALU2B("ALU2B", kAddrBusSize);

BusALU alu1("ALU1", kAddrBusSize);
BusALU alu2("ALU2", kAddrBusSize);

Counter pc("PC", kRegSize);
Counter mdr("MDR", kRegSize);
StorageObject ir("IR", kRegSize);
Clearable r0("R0", kRegSize);
Clearable r1("R1", kRegSize);
Clearable r2("R2", kRegSize);
Clearable r3("R3", kRegSize);
Clearable r4("R4", kRegSize);
Clearable r5("R5", kRegSize);
Clearable r6("R6", kRegSize);
Clearable r7("R7", kRegSize);
Clearable r8("R8", kRegSize);
Clearable r9("R9", kRegSize);
Clearable rA("RA", kRegSize);
Clearable rB("RB", kRegSize);
Clearable pR("PR", kRegSize);
Clearable pB("PB", kRegSize);
Clearable pS("PS", kRegSize);

Memory m("Main Memory", kAddrBusSize, kDataBusSize);

//=========================================<uPU Objects>==========================================//
Memory micro_m("Micro Control Storage", kAddrBusSize, kDataBusSize);

void connect() {
       m.MAR().connectsTo(ALU1Out.OUT());
       m.MAR().connectsTo(ALU2Out.OUT());	

       mdr.connectsTo(ALU1Out.OUT());
       mdr.connectsTo(ALU2Out.OUT());
       mdr.connectsTo(m.WRITE());
       mdr.connectsTo(m.READ());
       mdr.connectsTo(ALU1A.IN());
       mdr.connectsTo(ALU1B.IN());
       mdr.connectsTo(ALU2A.IN());
       mdr.connectsTo(ALU2B.IN());
}

void execute(const char * codeFile, const char * uCodeFile) {
    
}
