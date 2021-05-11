#ifndef  _CPU_H_
#define  _CPU_H_

#include <math.h>

#include "includes.h"

enum sys_names {
    uPC = 0,
    uRET,
    uR0,
    uR1,
    uR2,
    uCnt,
    MDR,
    MAR, 
    OP1Val,
    OP1Type,
    OP2Val,
    OP2Type,
    OP3Val,
    OP3Type,
    OP4Val,
    OP4Type,
    OP1Scale,
    OP2Scale,
    OP3Scale,
    OP4Scale,
    IR,
    uTmp,
    uRet2,
    uTmp2,
    Carry,
    Overflow,
};

const size_t kAddrBusSize = 32;
const size_t kDataBusSize = 8;
const size_t kRegSize = 32;

const size_t CU_DATA_SIZE = 55;

const size_t kMaxAddr = int(pow(2, 20)) - 1;

const long WRITEBACK_UPC = 0x3FF;

// Memory
extern Memory m;
extern Memory control_storage;

// ALUs
extern BusALU ALU1;
extern BusALU ALU2;

void connect();

void execute(const char * codeFile, const char * uCodeFile, bool doUTrace);

#endif //_CPU_H_
