.data

.code 
start: 
    add %r6, $0x7FFFFFFF, $0xAFFFFFFF
    bac success
    hlt

success:
    add %r7, $0x1FFFFFFF, $0x7FFFFFFF
    bao success2
    dmp %r6
    hlt

success2:
    dmp %r7
    hlt

