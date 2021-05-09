.data 

.code
start:
    # We will use r3 as the return reg
    mov %r9, $0xFFFFF
    mov %rA, $10
    jal fib
    dmp %r3
    hlt

fib:
    # push rA and 2 regs on the stack
    sub %r9, %r9, $0x14
    mov 0(%r9), %pR
    mov 4(%r9), %r0
    mov 8(%r9), %r1
    mov 12(%r9), %r2
    mov 16(%r9), %rA
    
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
    mov %pR, 0(%r9) 
    mov %r0, 4(%r9) 
    mov %r1, 8(%r9) 
    mov %r2, 12(%r9) 
    mov %rA, 16(%r9) 
    add %r9, %r9, $0x14
    ret

