typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern u8 *data_020a9db8;

extern void func_02065c2c(u8 *a, u8 *b);
extern int func_02065de4(void *a, u32 b);
extern void func_0205a61c(void *dst, void *src, int n);
extern int func_02066fec(u32 a, int b, int c);
extern u16 func_02065ef8(u16 a, u16 b);

#define LD(t, x) ((t *)((((long long)(int)(x))) & 0xFFFFFFFFFFFFFFFFLL))

void func_0206655c(void *arg0, u32 arg1)
{
    u8 buf[0x38];
    int phase;
    int idx;
    u8 cnt;
    u8 physIdx;
    u8 *rec;
    u8 *p;
    u8 bidx;
    u16 flags;
    u32 bit;
    u8 pi;
    u8 *b;
    u16 *fld;
    int gv;
    u8 *cp;
    u32 i;

    func_02065c2c((u8 *)arg0 + 0xa, buf);
    if (arg1 == 0)
        return;
    if (arg1 > 0xf)
        return;

    idx = arg1 - 1;
    phase = ((int *)(data_020a9db8 + 0x14e8))[idx];

    switch (buf[0]) {
    case 7:
        if (phase == 2 && func_02065de4(buf, arg1) == 1) {
            i = arg1 - 1;
            func_0205a61c(data_020a9db8 + 0x1538 + i * 32, buf + 0x14, 0x1d);
            ((int *)(data_020a9db8 + 0x14a8))[i] = *(int *)(buf + 0x14);
            *(u16 *)(data_020a9db8 + i * 2 + 0x148a) = *(u16 *)(buf + 0x2e);
            func_0205a61c(buf + 0x18, data_020a9db8 + 0x1340 + i * 0x16, 0x16);
            {
                u8 *pp = data_020a9db8 + 0x1340 + i * 0x16;
                *pp = (u8)((*pp & ~0xf0) | (((u8)arg1 & 0xf) << 4));
            }
            func_02066fec(arg1, 0xa, (int)(buf + 0x18));
        }
        if (phase != 0xa)
            return;
        cnt = 0;
        physIdx = *(u8 *)(data_020a9db8 + idx * 0x20 + 0x1554);
        if (physIdx < 0x10) {
            rec = data_020a9db8 + physIdx * 0x5c4;
            if (*(u8 *)(rec + 0x1d4a) != 0) {
                p = *(u8 **)(rec + 0x1d38);
                gv = ((int *)(data_020a9db8 + 0x14a8))[idx];
                if (gv == *(int *)(p + 0x14))
                    goto have;
            }
        }
        *(u16 *)(data_020a9db8 + idx * 2 + 0x1754) = 4;
        goto disp;
    have:
        flags = *(u16 *)(rec + 0x1d46);
        for (bidx = 0; (u32)bidx < 0x10; bidx++) {
            if ((1 << bidx) & flags)
                cnt++;
        }
        if ((u32)cnt >= (u32)*(u8 *)(p + 0x18)) {
            *(u16 *)(data_020a9db8 + idx * 2 + 0x1754) = 0;
            func_02066fec(arg1, 0xb, 0);
            return;
        }
    disp:
        b = data_020a9db8 + idx * 2 + 0x1700;
        switch (*(u16 *)(b + 0x54)) {
        case 3:
            bit = 1u << arg1;
            if (*(u16 *)(data_020a9db8 + 0x1536) & bit)
                return;
            cp = LD(u8, data_020a9db8 + 0x1535);
            (*cp)++;
            fld = LD(u16, data_020a9db8 + 0x1536);
            *fld = (u16)(*fld | bit);
            *(u8 *)(data_020a9db8 + (arg1 - 1) + 0x1526) = physIdx;
            fld = LD(u16, data_020a9db8 + physIdx * 0x5c4 + 0x1d46);
            *fld = (u16)(*fld | bit);
            fld = LD(u16, data_020a9db8 + physIdx * 0x5c4 + 0x1d48);
            *fld = (u16)(*fld | bit);
            *(u16 *)(data_020a9db8 + (arg1 - 1) * 2 + 0x1754) = 0;
            func_02066fec(arg1, 5, 0);
            return;
        case 4:
            *(u16 *)(b + 0x54) = 0;
            func_02066fec(arg1, 4, 0);
            return;
        }
        return;

    case 8:
        if (phase == 5) {
            func_02066fec(arg1, 0xe, 0);
            return;
        }
        if (phase != 0xe)
            return;
        if (*(u16 *)(data_020a9db8 + idx * 2 + 0x1754) != 2)
            return;
        pi = *(u8 *)(data_020a9db8 + idx + 0x1526);
        fld = LD(u16, data_020a9db8 + pi * 0x5c4 + 0x1d44);
        *fld = (u16)(*fld | (1u << arg1));
        *(u16 *)(data_020a9db8 + pi * 0x5c4 + 0x1d40) = 0;
        *(u16 *)(data_020a9db8 + idx * 2 + 0x1754) = 0;
        func_02066fec(arg1, 6, 0);
        return;

    case 9:
        if (phase != 6)
            return;
        pi = *(u8 *)(data_020a9db8 + idx + 0x1526);
        if (pi == 0xff)
            return;
        *(u16 *)(data_020a9db8 + pi * 0x5c4 + 0x1d42) =
            func_02065ef8(*(u16 *)(data_020a9db8 + pi * 0x5c4 + 0x1d42), *(u16 *)(buf + 2));
        return;

    case 10:
        if (phase == 6) {
            pi = *(u8 *)(data_020a9db8 + idx + 0x1526);
            if (pi == 0xff)
                return;
            fld = LD(u16, data_020a9db8 + pi * 0x5c4 + 0x1d44);
            *fld = (u16)(*fld & ~(1u << arg1));
            func_02066fec(arg1, 7, 0);
            return;
        }
        if (phase != 7)
            return;
        b = data_020a9db8 + idx * 2 + 0x1700;
        if (*(u16 *)(b + 0x54) != 5)
            return;
        *(u16 *)(b + 0x54) = 0;
        func_02066fec(arg1, 8, 0);
        return;

    case 11:
        break;
    }
}
