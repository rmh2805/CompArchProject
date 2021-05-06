# Control Instructions

# HLT
    0x00 sub pc i0 i1; goto Writeback       # Set up PC overflow
# NOP
    0x01 none; goto Writeback
# CLR
    0x02 mov uR0 i0; goto Writeback
# DMP
    0x03 mov uR0 OP1Val; goto Writeback
# OUT
    0x04 mov o1 OP1Val; goto Writeback
