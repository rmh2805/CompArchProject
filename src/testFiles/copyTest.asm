.data

.code
start:
    
    mov %r6, $0xFF
    mov %r5, $4
    mov %r4, $0

push_str:
    bge %r4, %r5, push_str_done
    mul %r3, %r4, $4
    add %r0, %r3, %r6
    mov (%r0), $0xFFFFFFFF 
    add %r4, %r4, $1
    jmp push_str

push_str_done:
    dmp 16(%r6)
    dmp 20(%r6)
    dmp 24(%r6)
    dmp 28(%r6)
    cpy 0xFF, 16(%r6), $0, $16
    dmp 16(%r6)
    dmp 20(%r6)
    dmp 24(%r6)
    dmp 28(%r6)
    hlt
