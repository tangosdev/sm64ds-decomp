typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;

extern char *data_020a9db8;

extern int func_02066fec(int a, int b, int c);
extern void func_0205a588(void *dst, int val, u32 size);
extern int func_0206881c(void);
extern void func_020664b4(int a);
extern void func_02065f78(void);
extern void func_0206853c(char *self, void *src, u32 flags, int s);
extern void func_02068410(void);
extern void func_02065eb8(void *a, short s);
extern void MultiStore32Bytes(u32 val, void *dst, int len);

void func_02066a94(int arg0, char *arg1)
{
    switch (arg0) {
    case 0x15:
        func_02066fec(0, 1, (int)arg1);
        break;
    case 0x11:
        func_02066fec(0, 0xc, 0);
        break;
    case 0: {
        u16 v = *(u16 *)(arg1 + 0x10);
        if (v != 0 && v < 0x10) {
            func_02066fec(v, 2, (int)arg1);
        }
        break;
    }
    case 1: {
        u16 v = *(u16 *)(arg1 + 0x10);
        if (v != 0 && v < 0x10) {
            *(u16 *)(data_020a9db8 + (v - 1) * 2 + 0x148a) = 0;
            func_0205a588(data_020a9db8 + 0x14a8 + (*(u16 *)(arg1 + 0x10) - 1) * 4, 0, 4);
            func_0205a588(data_020a9db8 + 0x1340 + (*(u16 *)(arg1 + 0x10) - 1) * 0x16, 0, 0x16);
            {
                int cur = func_0206881c();
                func_0205a588(data_020a9db8 + 0x1538 + ((*(u16 *)(arg1 + 0x10) - 1) << 5), 0,
                              (cur - 2) * 0x1e / (func_0206881c() - 2));
            }
            *(int *)(data_020a9db8 + (*(u16 *)(arg1 + 0x10) - 1) * 4 + 0x1718) = 0;
            {
                u16 id = *(u16 *)(arg1 + 0x10);
                s8 slot = *(s8 *)(data_020a9db8 + (id - 1) + 0x1526);
                if (slot != -1) {
                    u32 off = (u8)slot * 0x5c4;
                    u32 mask = ~(1 << id);
                    *(u16 *)(((long long)(int)(data_020a9db8 + off + 0x1d46)) & 0xFFFFFFFFFFFFFFFFLL) &= mask;
                    *(u16 *)(((long long)(int)(data_020a9db8 + off + 0x1d48)) & 0xFFFFFFFFFFFFFFFFLL) |= (1 << id);
                    *(s8 *)(data_020a9db8 + (id - 1) + 0x1526) = -1;
                    *(u16 *)(((long long)(int)(data_020a9db8 + off + 0x1d44)) & 0xFFFFFFFFFFFFFFFFLL) &= mask;
                }
            }
            if (*(u16 *)(data_020a9db8 + 0x1536) & (1 << *(u16 *)(arg1 + 0x10))) {
                *(u8 *)(((long long)(int)(data_020a9db8 + 0x1535)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
                *(u16 *)(((long long)(int)(data_020a9db8 + 0x1536)) & 0xFFFFFFFFFFFFFFFFLL) &= ~(1 << *(u16 *)(arg1 + 0x10));
            }
            {
                u16 id2 = *(u16 *)(arg1 + 0x10);
                if (*(int *)(data_020a9db8 + (id2 - 1) * 4 + 0x14e8) == 8) {
                    func_02066fec(id2, 9, 0);
                }
            }
            func_02066fec(*(u16 *)(arg1 + 0x10), 3, (int)arg1);
            *(int *)(data_020a9db8 + (*(u16 *)(arg1 + 0x10) - 1) * 4 + 0x14e8) = 0;
        }
        break;
    }
    case 3:
        func_020664b4((int)arg1);
        break;
    case 0x19:
        func_02065f78();
        break;
    case 0x1c: {
        u8 i;
        for (i = 0; i < 0x10; i++) {
            char *e = data_020a9db8 + i * 0x5c4;
            if (*(u8 *)(e + 0x1d4a) != 0) {
                u16 f48 = *(u16 *)(e + 0x1d48);
                if (f48 != 0) {
                    func_0206853c(data_020a9db8 + 0x186c + i * 0x5c4, data_020a9db8 + 0x1340,
                                  *(u16 *)(e + 0x1d46), f48);
                    *(u16 *)(data_020a9db8 + i * 0x5c4 + 0x1d48) = 0;
                }
            }
        }
        func_02068410();
        break;
    }
    case 0xff: {
        u16 v = *(u16 *)(arg1 + 2);
        switch (v) {
        case 1:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
            func_02065eb8(0, 9);
            break;
        default:
        case 0:
        case 2:
        case 3:
        case 7:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            func_02065eb8(0, 8);
            break;
        }
        break;
    }
    case 0x100: {
        u16 v = *(u16 *)arg1;
        switch (v) {
        case 0:
        case 7:
        case 8:
        case 13:
        case 14:
        case 15:
        case 17:
        case 18:
        case 21:
        case 25:
        case 29:
            func_02065eb8(0, 9);
            break;
        default:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 9:
        case 10:
        case 11:
        case 12:
        case 16:
        case 19:
        case 20:
        case 22:
        case 23:
        case 24:
        case 26:
        case 27:
        case 28:
            func_02065eb8(0, 8);
            break;
        }
        break;
    }
    }

    if (arg0 != 0x11) {
        return;
    }
    {
        volatile int sp0 = 0;
        MultiStore32Bytes(sp0, data_020a9db8, 0x73e0);
    }
    data_020a9db8 = 0;
}
