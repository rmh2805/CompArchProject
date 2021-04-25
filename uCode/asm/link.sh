#! /bin/bash
./assemble.sh fetch.asm 0 > fetch.obj
./assemble.sh GetOperand.asm 100 > GetOperand.obj
./assemble.sh Decode.asm 200 > Decode.obj
./assemble.sh EvalOp.asm 300 > EvalOp.obj
./assemble.sh RegVal.asm A0 > RegVal.obj 
python3 ../../Asm/uAsm/link.py uCode.obj 0 fetch.obj 0 RegVal.obj A0 GetOperand.obj 100 Decode.obj 200 EvalOp.obj 300 