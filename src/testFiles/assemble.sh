#! /bin/bash
cat $1 | ../../Asm/Assembler/tokenizer | python3 ../../Asm/Assembler/worseVAX.py 
