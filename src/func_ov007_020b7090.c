typedef unsigned short u16;

typedef struct S {
    char pad0[0x10];
    int f10;                 // 0x10
    int f14;                 // 0x14
    char pad1[0x50 - 0x18];
    int f50;                 // 0x50
    int f54;                 // 0x54
} S;

extern S *data_ov007_0210342c;
extern int data_ov007_02103430;

extern void func_ov007_020c20b8(int a, void *b);
extern void func_ov007_020b2e64(u16 *buf);
extern void func_ov007_020c1db0(void *c, u16 *in);
extern void *func_ov007_020bdebc(void);
extern void _ZN5EnemyC2Ev(void *thiz);

int func_ov007_020b7090(u16 a0, u16 a1, u16 a2, u16 a3, int arg4)
{
    u16 buf[4];
    int ret = 0;

    data_ov007_02103430 = 0;
    func_ov007_020c20b8(data_ov007_0210342c->f54, &arg4);
    buf[0] = a0;
    buf[1] = a1;
    buf[2] = a2;
    buf[3] = a3;
    func_ov007_020b2e64(buf);
    func_ov007_020c1db0((void *)data_ov007_0210342c->f50, buf);
    _ZN5EnemyC2Ev(func_ov007_020bdebc());
    {
        S *ip = data_ov007_0210342c;
        if (ip->f14 != 0 || ip->f10 == 1)
            ret = ip->f10;
    }
    return ret;
}
