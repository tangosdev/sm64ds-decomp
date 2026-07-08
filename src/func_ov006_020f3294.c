// NONMATCHING: different op / idiom (div=32). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern void func_ov004_020adb1c(int self);
extern void func_ov004_020b0aa0(int arg);
extern void func_ov006_020f2ec0(char* c);
extern void func_ov006_020f2e20(char* c);
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
extern char* func_020beb68;
extern unsigned char TOP_SCREEN_RELATED;
extern unsigned char BOTTOM_SCREEN_RELATED;

void func_ov006_020f3294(char* c, int arg1)
{
    if (*(unsigned char*)(c + 0x5000 + 0x459) != 0) {
        *(unsigned char*)(c + 0x5457) = *(unsigned char*)(c + 0x5457) + 1;
        *(int*)(c + 0xbc) = *(int*)(c + 0xbc) + 1;
        if ((unsigned int)*(int*)(c + 0xbc) > 0x270e) *(int*)(c + 0xbc) = 0x270e;
    }
    if (arg1 == 0x12) {
        *(short*)(c + 0x5100 + 0x72) = 0xa;
        *(int*)(c + 0xbc) = 0;
        if ((unsigned int)*(int*)(c + 0xbc) > 0x270e) *(int*)(c + 0xbc) = 0x270e;
        if (func_020beb68 != 0) *(int*)(func_020beb68 + 0xb4) = 0;
        func_ov004_020adb1c(func_020beb68 != 0 ? *(int*)(func_020beb68 + 0xb4) : 0);
    }
    func_ov004_020b0aa0(0x1d);
    func_ov006_020f2ec0(c);
    func_ov006_020f2e20(c);
    *(int*)(c + 0x4000 + 0xf78) = 0;
    TOP_SCREEN_RELATED |= 4;
    BOTTOM_SCREEN_RELATED &= ~4;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
}
