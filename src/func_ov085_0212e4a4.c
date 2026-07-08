//cpp
// NONMATCHING: constant / value (div=19). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct V3 { int x, y, z; };

extern "C" int _ZN5Actor13ClosestPlayerEv(void);
extern "C" int _ZN6Player17SetNoControlStateEhih(int self, unsigned char a, int b, unsigned char c);
extern "C" void func_ov085_0212e728(int c, int a, int b);

extern int SAVE_DATA[];
extern int data_ov085_02130800[];

extern "C" int func_ov085_0212e4a4(int self)
{
    int p = _ZN5Actor13ClosestPlayerEv();
    if (p != 0) {
        V3 v = *(V3*)(p + 0x5c);
        if ((SAVE_DATA[2] & 0x10000) != 0 &&
            *(int*)((int)&v + 8) > -0x28000 &&
            _ZN6Player17SetNoControlStateEhih(p, 0x12, -1, 0) != 0) {
            *(int*)(p + 0x744) = *(int*)(self + 0x5c);
            *(int*)(p + 0x748) = *(int*)(self + 0x60);
            *(int*)(p + 0x74c) = *(int*)(self + 0x64);
            *(int*)(self + 0x5c) = v.x;
            *(int*)(self + 0x60) = v.y;
            *(int*)(self + 0x64) = v.z;
            {
                int *q = (int*)(self + 0x5c);
                *q = *q - 0x3e8000;
            }
            *(int*)(self + 0x60) = *(int*)(p + 0x644) + 0x3e8000;
            *(int*)(self + 0x2a4) = *(int*)(self + 0x5c);
            *(int*)(self + 0x2a8) = *(int*)(self + 0x60);
            *(int*)(self + 0x2ac) = *(int*)(self + 0x64);
            *(signed char*)(self + 0x2dc) = 0;
            func_ov085_0212e728(self, (int)data_ov085_02130800, 0);
        }
    }
    return 1;
}
