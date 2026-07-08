// NONMATCHING: behaviorally complete, but the ROM keeps ~4 words of UNREACHABLE dead
// code after the hang - a trailing `ldr;str;bx` with NO function epilogue (the pushed
// r4-r6/lr are never restored), i.e. dead code the ROM's compiler emitted but did not
// clean. mwccarm 1.2/sp2p3 strips everything after the provably-infinite loop at every
// opt level, so the tail cannot be reproduced from C. All reachable code matches.
//
// Spin calling the IPC-style primitive with (0xd, 2, 0) until it returns 0, then hang.
extern int func_0205b988(int, int, int);
void func_020610fc(void)
{
    while (func_0205b988(0xd, 2, 0) != 0);
    for (;;);
}
