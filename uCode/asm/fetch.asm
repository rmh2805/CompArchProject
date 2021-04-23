// Fetch:
0 none none

// Increment PC
1 if pcmax goto halt
2 add PC pc i1; none

// Move instruction into IR
3 mov mar pc; none
4 read; none
5 mov ir mdr; goto fetchoperands

