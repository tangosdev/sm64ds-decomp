typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct G9d2c {
    u8 pad0[4];      /* 0 */
    u8 *p;           /* 4 */
    u8 *cur;         /* 8 */
    u8 state;        /* 0xc */
    u8 pad_d;        /* 0xd */
    u8 e;            /* 0xe */
    u8 f;            /* 0xf */
    u8 g;            /* 0x10 */
    u8 h;            /* 0x11 */
};
extern struct G9d2c data_020a9d2c;

struct Pkt {
    u32 f0;          /* 0 */
    u8 f4;           /* 4 */
    u8 f5;           /* 5 */
    u8 f6;           /* 6 */
    u8 f7;           /* 7 */
    u16 f8;          /* 8 */
    u8 fa;           /* 0xa */
    u8 fb;           /* 0xb */
    u8 fc;           /* 0xc */
    u8 fd;           /* 0xd */
    u8 body[0x62];   /* 0xe */
};
extern struct Pkt data_020a9d40;
extern u8 data_020a9d4e[];
extern u16 data_020a9d48[];

extern void MultiStore16(u16 val, void *dst, int nbytes);
extern void MultiCopyHalf(const void *src, void *dst, int count);
extern u16 func_02067ee4(u16 *p, int len);
extern int func_020688e4(void);
extern int func_020688cc(void);
extern int func_020688b4(void);
extern int func_02068844(void);
extern int func_02063648(int a0, int *src, int len, int a3, int arg5, int arg6, u8 arg7);

void func_020681b8(void)
{
    volatile u16 v;

    {
        u8 *p = data_020a9d2c.p;
        u8 *cur = data_020a9d2c.cur;
        unsigned lim = (unsigned)p + 0x358;
        unsigned nxt = (unsigned)cur + 0x62;
        if (nxt <= lim) {
            data_020a9d40.fc = 0x62;
        } else {
            data_020a9d40.fc = (u8)(lim - (unsigned)cur);
            v = 0;
            MultiStore16(v, data_020a9d4e + data_020a9d40.fc, 0x62 - data_020a9d40.fc);
        }
    }
    MultiCopyHalf(data_020a9d2c.cur, data_020a9d4e, data_020a9d40.fc);

    data_020a9d40.fa = data_020a9d2c.f;
    data_020a9d40.fb = data_020a9d2c.g;
    data_020a9d40.f4 = (data_020a9d40.f4 & ~3) | (data_020a9d2c.p[0x4aa] & 3);
    data_020a9d40.f5 = data_020a9d2c.p[0x4ab];
    data_020a9d40.f6 = data_020a9d2c.e;
    data_020a9d40.f0 = *(u32 *)(data_020a9d2c.p + 0x4b0);
    data_020a9d40.f4 = (data_020a9d40.f4 & ~0xfc) | ((data_020a9d2c.p[0x4ad] & 0x3f) << 2);
    data_020a9d40.f7 = data_020a9d2c.h++;
    data_020a9d40.f8 = 0;
    data_020a9d40.f8 = func_02067ee4(data_020a9d48, 0x68);

    if (++data_020a9d2c.f < data_020a9d2c.g)
        data_020a9d2c.cur += 0x62;
    else
        data_020a9d2c.state = 4;

    func_02063648(0, (int *)&data_020a9d40, 0x70,
                  func_020688e4(), func_020688cc(), func_020688b4(),
                  func_02068844() | 2);
}