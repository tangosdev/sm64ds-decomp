typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern u8 data_020a9d2c[];
extern u8 data_020a9d40[];
extern u16 data_020a9d48[];
extern u8 data_020a9d50[];

extern void func_02068188(void);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void MultiCopyHalf(void *src, void *dst, int n);
extern u16 func_02067ee4(u16 *p, int len);
extern int func_020688e4(void);
extern int func_020688cc(void);
extern int func_020688b4(void);
extern int func_02068844(void);
extern int func_02063648(int a0, int *src, int len, int a3, int arg5, int arg6, u8 arg7);

void func_02067f68(void)
{
    u8 *ptr;
    volatile u16 zero;
    u16 mask;
    int i;
    u32 count;
    u8 *dst;
    int a;
    int b;
    int c;
    u8 seq;

    if (data_020a9d2c[0xe] != (*(u8 **)(data_020a9d2c + 4))[0x4ac]) {
        func_02068188();
    }

    ptr = *(u8 **)(data_020a9d2c + 4);
    data_020a9d40[4] = (data_020a9d40[4] & ~3) | 2;
    data_020a9d40[5] = ptr[0x4ab];
    data_020a9d40[6] = data_020a9d2c[0xe];
    *(u32 *)data_020a9d40 = *(u32 *)(ptr + 0x4b0);
    data_020a9d40[4] = (data_020a9d40[4] & ~0xfc) | ((ptr[0x4ad] & 0x3f) << 2);
    seq = data_020a9d2c[0x11];
    data_020a9d2c[0x11] = seq + 1;
    data_020a9d40[7] = seq;
    data_020a9d40[0xa] = ptr[0x358];
    *(u16 *)(data_020a9d40 + 0xc) = *(u16 *)(ptr + 0x35a);
    *(u16 *)(data_020a9d40 + 0xe) = *(u16 *)(ptr + 0x35c);

    zero = 0;
    MultiStore16(zero, (char *)data_020a9d50, 0x58);

    ptr = *(u8 **)(data_020a9d2c + 4);
    mask = *(u16 *)(ptr + 0x4a8) ^ *(u16 *)(ptr + 0x35a);
    count = 0;
    dst = data_020a9d50;
    for (i = 0; i < 15; i++) {
        if (mask & (2 << i)) {
            MultiCopyHalf(*(u8 **)(data_020a9d2c + 4) + 0x35e + i * 0x16, dst, 0x16);
            count++;
            dst += 0x16;
            *(u16 *)(((int)*(u8 **)(data_020a9d2c + 4) + 0x4a8)) |= 2 << i;
            if (count == 4) {
                break;
            }
        }
    }

    if (count < 4) {
        data_020a9d50[count * 0x16] &= ~0xf0;
    }

    *(u16 *)(data_020a9d40 + 8) = 0;
    *(u16 *)(data_020a9d40 + 8) = func_02067ee4(data_020a9d48, 0x68);

    ptr = *(u8 **)(data_020a9d2c + 4);
    if (*(u16 *)(ptr + 0x4a8) == *(u16 *)(ptr + 0x35a)) {
        data_020a9d2c[0xc] = 1;
    }

    a = func_020688e4();
    b = func_020688cc();
    c = func_020688b4();
    func_02063648(0, (int *)data_020a9d40, 0x70, a, b, c, func_02068844() | 2);
}
