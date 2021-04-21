#ifndef  _CPU_H_
#define  _CPU_H_

#include <math.h>

#include "includes.h"

const size_t kAddrBusSize = 32;
const size_t kDataBusSize = 8;
const size_t kRegSize = 32;

const size_t kMaxAddr = int(pow(2, kAddrBusSize)) - 1;

// Buses
extern Bus ALU1Out;
extern Bus ALU2Out;
extern Bus ALU1A;
extern Bus ALU1B;
extern Bus ALU2A;
extern Bus ALU2B;

// Memory
extern Memory m;
extern Memory micro_m;

// ALUs
extern BusALU alu1;
extern BusALU alu2;

// Registers
extern Counter pc;
extern Counter mdr;
extern StorageObject ir;
extern Clearable r0;
extern Clearable r1;
extern Clearable r2;
extern Clearable r3;
extern Clearable r4;
extern Clearable r5;
extern Clearable r6;
extern Clearable r7;
extern Clearable r8;
extern Clearable r9;
extern Clearable rA;
extern Clearable rB;
extern Clearable pR;
extern Clearable pB;
extern Clearable pS;

void connect();

void execute(const char * codeFile, const char * uCodeFile);

#endif //_CPU_H_
