extern void* _ZN8dActor_c13ClosestPlayerEv(void);
extern void _Z14ApproachLinearRiii(int* p, int a, int b);
extern int func_ov002_020c3ea0(void* c);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3s(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int e);
extern void func_ov085_0212e728(void* c, void* p);
extern short data_02082214[];
extern short data_ov085_02130820;

int func_ov085_0212d9b8(char* c)
{
    void* pl = _ZN8dActor_c13ClosestPlayerEv();
    if (pl == 0) return 1;

    *(int*)(((int)c + 0x2c8)) += 1;
    *(int*)(((int)c + 0x2cc)) += 0x500;

    {
        int v = (short)*(int*)(c + 0x2cc);
        int idx = (int)((unsigned int)(v << 16) >> 16) >> 4;
        short e = data_02082214[idx * 2];
        _Z14ApproachLinearRiii((int*)(c + 0x60),
            (int)(((long long)e * 0x1a000 + 0x800) >> 12) + *(int*)(c + 0x2a8),
            0x10000000);
    }

    if (*(int*)(c + 0x2c8) == 0x32) {
        func_ov002_020c3ea0(pl);
    }
    *(short*)(c + 0x8e) = 0x6000;
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);
    {
        int a1 = 0x4000;
        if (*(int*)(c + 0x2c8) >= 0x4b) a1 = 0x8000;
        _Z14ApproachLinearRiii((int*)(c + 0x98), a1, 0x1000);
    }

    *(int*)(c + 0x2e4) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        *(unsigned int*)(c + 0x2e4), 3, 0x182, (void*)(c + 0x74), 0);

    if (*(int*)(c + 0x2c8) > 0x78) {
        *(short*)(c + 0x8e) = 0x4000;
        *(short*)(c + 0x94) = *(short*)(c + 0x8e);
        func_ov085_0212e728(c, &data_ov085_02130820);
    }
    return 1;
}
