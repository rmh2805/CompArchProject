.data 

.code
start:
    # We will use r3 as the return reg
    mov %pS, $0xFFFF
    mov %pB, $0xFFFF
    mov %rA, $10
    jal fib
    dmp %r3
    hlt

fib:
    # push rA and 2 regs on the stack
    sub %pS, %pS, $20
    mov 0(%pS), %pR
    mov 4(%pS), %r0
    mov 8(%pS), %r1
    mov 12(%pS), %r2
    mov 16(%pS), %rA

    # Base Case
    mov %r3, %rA
    bge $1, %rA, fibRet

    # Revursive calls
    sub %rA, %rA, $1
    jal fib
    mov %r1, %r3
    sub %rA, %rA, $1
    jal fib
    add %r0, %r1, %r3
    mov %r3, %r0

fibRet:
    mov %pR, 0(%pS) 
    mov %r0, 4(%pS) 
    mov %r1, 8(%pS) 
    mov %r2, 12(%pS) 
    mov %rA, 16(%pS) 
    add %pS, %pS, $20
    ret

