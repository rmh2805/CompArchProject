#! /bin/bash
cat $1 | ../../Asm/uAsm/tokenizer | python3 ../../Asm/uAsm/assemble.py loadPoints.txt