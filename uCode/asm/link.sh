#! /bin/bash
./assemble.sh fetch.asm > fetch.obj
./assemble.sh GetOperand.asm > GetOperand.obj
python3 ../../Asm/uAsm/link.py uCode.obj 0 fetch.obj 0 GetOperand.obj FF