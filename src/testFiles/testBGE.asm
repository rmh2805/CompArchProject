.data
.code
    mov %rA, $0x00
    bge $0x01, %rA, success

failure:
    dmp $0
    hlt

success:
    dmp $1
    
    bge $0x01, $0x01, success2
    jmp failure

success2:
    dmp $1
    
    bge $0x01, $0x02, success3
    jmp failure

success3:
    dmp $1
    hlt
