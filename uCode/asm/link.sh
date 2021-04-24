#! /bin/bash
./assemble.sh fetch.asm > fetch.obj
./assemble.sh GetOperand.asm > GetOperand.obj
./assemble.sh Decode.asm > Decode.obj
python3 ../../Asm/uAsm/link.py uCode.obj 0 fetch.obj 0 GetOperand.obj 100 Decode.obj 200