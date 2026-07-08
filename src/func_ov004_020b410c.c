// NONMATCHING: register allocation (div=26). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int func_02053200(int x);
extern void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov004_020b1cf0(int a, int b, int c, int sel, int e);
extern short SINE_TABLE[];
extern int data_ov006_021346bc;

struct L4 { int a, b, cc, d; };

void func_ov004_020b410c(char* c)
{
    struct L4 local;
    short* base = (short*)(c + 0x34);
    short* p;
    int x = *(int*)(c + 0x24);
    int idx = (int)(((unsigned int)x << 15) >> 16) >> 4;
    short s = *(short*)((char*)SINE_TABLE + (idx << 2));
    int v = func_02053200((int)((((long long)s << 11) + 0x800) >> 12) + 0x1000);

    local.a = 0;
    local.b = 0;
    local.cc = 0;
    local.d = 0;
    local.a = v;
    local.d = v;

    func_ov004_020b1c68(base,
        *(short*)(c + 0x10), *(short*)(c + 0x12), *(int*)(c + 0x1c),
        *(int*)(c + 0x18), &local);

    p = base;
    do {
        s = p[3];
        p += 4;
    } while ((unsigned short)s != 0xffff);

    {
        int sel = *(int*)(c + 0x20);
        if (sel != 0 && sel != 1 && sel != 2) return;
    }
    func_ov004_020b1cf0(data_ov006_021346bc,
        *(short*)(c + 0x10), *(short*)(c + 0x12), *(int*)(c + 0x1c),
        *(int*)(c + 0x18));
}
