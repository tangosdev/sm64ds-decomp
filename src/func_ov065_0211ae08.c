typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* self);
extern void func_ov065_0211ac0c(void* c);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
extern u16 DecIfAbove0_Short(u16* p);
extern int RandomIntInternal(int* seed);
extern void func_ov065_0211ad70(char* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* c);

extern s16 data_02082214[];
extern u8 data_0209f2c0;
extern int data_0209e650;

#define I32(off) (*(int*)(((int)c + (off)) & 0xFFFFFFFFFFFFFFFFLL))
#define I8(off)  (*(u8*)(((int)c + (off)) & 0xFFFFFFFFFFFFFFFFLL))

int func_ov065_0211ae08(char* c)
{
    if (data_0209f2c0 == 3) {
        int ang;
        ang = *(u16*)(c + 0x94);
        *(int*)(c + 0x5c) = *(int*)(c + 0x320) +
            (int)(((long long)data_02082214[(ang >> 4) << 1] * 0xfa000 + 0x800) >> 12);
        ang = *(u16*)(c + 0x94);
        *(int*)(c + 0x64) = *(int*)(c + 0x328) +
            (int)(((long long)data_02082214[((ang >> 4) << 1) + 1] * 0xfa000 + 0x800) >> 12);
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
        func_ov065_0211ac0c(c);
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0)
            _ZN8Platform19UpdateClsnPosAndRotEv(c);
        return 1;
    }

    *(int*)(c + 0x330) = *(int*)(c + 0x32c);
    I32(0x32c) += *(int*)(c + 0x98);

    switch (*(u8*)(c + 0x336)) {
    case 0:
        if (DecIfAbove0_Short((u16*)(c + 0x334)) != 0) goto Lend;
        if (data_0209f2c0 == 2) {
            int v = (u16)((unsigned)RandomIntInternal(&data_0209e650) >> 16);
            if (v % 2 == 0) {
                *(s16*)(c + 0x334) = v % 100 + 20;
            }
        }
        I8(0x336)++;
        *(int*)(c + 0x98) = -0x8000;
        goto Lend;

    case 1:
        I32(0x98) += 0xbae;
        if (*(int*)(c + 0x98) > 0) {
            if (DecIfAbove0_Short((u16*)(c + 0x334)) != 0) {
                *(int*)(c + 0x98) = 0;
                goto Lend;
            }
            I8(0x336)++;
            *(int*)(c + 0x98) = 0x1d000;
        }
        goto Lend;

    case 2: {
        int d = *(int*)(c + 0x32c);
        if (d != 0xfa000) {
            int val = (int)(((long long)(0xfa000 - d) * (0xfa000 - *(int*)(c + 0x330)) + 0x800) >> 12);
            if (val >= 0) goto L280;
        }
        {
            int sp = *(int*)(c + 0x98);
            if (sp <= -0x8000) goto L280;
            if (sp >= 0x8000) goto L280;
            I8(0x336)++;
            *(int*)(c + 0x98) = 0;
            *(s16*)(c + 0x334) = 0x1e;
            goto Lend;
        }
      L280:
        {
            int m = (*(int*)(c + 0x32c) < 0xfa000) ? 0x6666 : -0x6666;
            int sp = *(int*)(c + 0x98);
            int delta;
            if ((int)(((long long)sp * m + 0x800) >> 12) >= 0)
                delta = m;
            else
                delta = (int)(((long long)m * 0x2599 + 0x800) >> 12);
            I32(0x98) += delta;
            if (data_0209f2c0 != 2) goto Lend;
        }
        if ((int)(((long long)*(int*)(c + 0x32c) * *(int*)(c + 0x330) + 0x800) >> 12) >= 0) goto Lend;
        if ((RandomIntInternal(&data_0209e650) & 3) != 0) goto Lend;
        func_ov065_0211ad70(c);
        goto Lend;
    }

    case 3:
        if (DecIfAbove0_Short((u16*)(c + 0x334)) != 0) goto Lend;
        *(int*)(c + 0x98) = -0x5000;
        if (*(int*)(c + 0x32c) < 0)
            func_ov065_0211ad70(c);
        goto Lend;
    }

Lend:
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    func_ov065_0211ac0c(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x1c0000, 0x1000) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
