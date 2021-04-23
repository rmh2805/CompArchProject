0 none; none
1 mov upc r1; mov r0 r1
2 sll mar pB i1; cmp r0 r0
3 srl mar r0 i7; read
4 or r0 r0 i9 ; write 
5 ext r0, r0; sub r4 r5 i1
6 add r1 r2 r3; xor r0 r0 r0
7 and op1type op1type i2; none

8 and r0 r0 r0; goto 0x50
9 add r0 r0 r0; goto decode

10 if Eq r0 i0 fetch
11 if nEq r0 i0 0x100
12 if Bits r0 0x0c 0x0
13 if nBits r0 0x03 0x0
14 if Byte r0 0x10 0x0
15 if nByte r0 0x10 0x0
16 if Nyb r0 0x10 0x0
17 if nNyb r0 0x10 0x0
18 if noOps 0x0
19 if oneOp 0x0
20 if twoOps 0x0
21 if threeOps 0x0
22 if pcmax decode
