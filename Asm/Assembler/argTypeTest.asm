.data
    a0 = .word 5
    a1 = .word 10
    a2 = .word 0x76543210
    memIndSto = .word 0

.code
start:
    # Register and immediate tests
    add %r0, %r2            # Test quick and slow register
    add %r0, $0x0F, $0x10   # Test nybble and full IMM
    
    # Absolute tests
    add a0, a0, $1          # Test absolute

    # Prepare indirect tests
    mov %r1, $a1            # Store a1's address in r1
    mov memIndSto, $a2      # Store a2's address in memory at memIndSto
    
    # Register indirect
    add (%r1), (%r1), $1

    # Memory indirect
    add @(memIndSto), @(memIndSto), $1

    # Index
    mov %r2, $4
    add (%r1 + %r2), (%r1 + %r2), $1

    # Displacement
    add 4(%r1), 4(%r1), $1

    # Increment and decrement
    add (%r0)+, (%r0)+, $1
    add -(%r0), -(%r0), $1