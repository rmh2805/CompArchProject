.data
    i0 = .word 0
.code
    mov %rA, i0
    bge $0x01, %rA, success

failure:
    dmp $0
    hlt

success:
    dmp $1
    hlt