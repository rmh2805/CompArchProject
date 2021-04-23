#! /bin/bash
make > /dev/null
cat $1 | ./tokenizer | python3 assemble.py loadPoints.txt
