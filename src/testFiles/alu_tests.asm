.data

.code
start:
    add %r1, %r1, $5
    add %r1, $7
    mul %r1, %r1, $3
    sub %r2, %r1, $7
    div %r3, %r2, $3
    xor %r3, $5, $3
    or %r3, $5, $3
    and %r3, $5, $3
    cmp %r4, %r3
    inc %r3
    mov %r3, $10
    mod %r3, %r3, $6

    mov %r5, $0x99
    mov (%r5), $100
    add %r3, $0, (%r5)

