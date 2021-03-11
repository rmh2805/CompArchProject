#ifndef  _CPU_H_
#define  _CPU_H_

#include <math.h>

#include "includes.h"

const size_t kAddrBusSize = 32;
const size_t kDataBusSize = 8;
const size_t kRegSize = 32;

const size_t kMaxAddr = int(pow(2, kAddrBusSize)) - 1;

void connect();

void execute(const char * codeFile, const char * uCodeFile);

#endif //_CPU_H_
