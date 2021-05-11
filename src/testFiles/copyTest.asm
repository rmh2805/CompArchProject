.data
    bnk = .byte 0x00 0x00 0x00 0x00 0x00 0x00
    src = .byte 0x01 0x02 0x03 0x02 0x01 0x00
    dst = .byte 0x00 0x00 0x00 0x00 0x00 0x00
    
.code
start:
    mov %r0, $0
    dmp dst(%r0)
    add dst, $4
    dmp dst(%r0)
    add dst, $4
    dmp dst(%r0)
    add dst, $4
    dmp dst(%r0)
    add dst, $4
    
    nop
    nop
    nop
    cpy src, dst
    
    mov %r0, $0
    dmp dst(%r0)
    add %r0, $4
    dmp dst(%r0)
    add %r0, $4
    dmp dst(%r0)
    add %r0, $4
    dmp dst(%r0)
    add %r0, $4
    
    nop
    nop
    nop
    cpy bnk, dst, $4
    
    mov %r0, $0
    dmp dst(%r0)
    add %r0, $4
    dmp dst(%r0)
    add %r0, $4
    dmp dst(%r0)
    add %r0, $4
    dmp dst(%r0)
    add %r0, $4
    
    nop
    nop
    nop
    cpy src, dst, $0, $3
    
    mov %r0, $0
    dmp dst(%r0)
    add %r0, $4
    dmp dst(%r0)
    add %r0, $4
    dmp dst(%r0)
    add %r0, $4
    dmp dst(%r0)
    add %r0, $4
    
    hlt
