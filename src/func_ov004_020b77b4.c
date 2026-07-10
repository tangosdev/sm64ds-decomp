//cpp
// NONMATCHING: register allocation (div=14). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern "C" {
void func_ov004_020b0aa0(int arg);
void func_ov004_020ae20c(void);
void func_ov004_020ae2c8(void);
int func_ov004_020b29a0(void* c, void* arg);
}

struct Vt { int dummy[0x4c/4]; int (*f)(void*, int); };
extern void* data_ov004_020beb68;

extern "C" void func_ov004_020b77b4(char* c){
    if (*(int*)(c+0x1c) != 0) return;
    void* r4 = data_ov004_020beb68;
    if (r4 == 0) return;
    {
        Vt* vt = *(Vt**)r4;
        if (vt->f(r4, *(int*)(c+0x18)) == 0) return;
    }
    func_ov004_020b0aa0(0x1d);
    func_ov004_020ae20c();
    func_ov004_020ae2c8();
    func_ov004_020b29a0(r4, *(void**)(c+0x18));
    *(int*)(c+0x1c) = 0;
    *(int*)(c+0x18) = -1;
    *(int*)(c+0x20) = 0;
    *(int*)(c+0x24) = 0;
}
