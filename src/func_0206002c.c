typedef unsigned short u16;

extern void MultiStore32Bytes(unsigned val, int *dst, int len);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
extern void func_02058200(void *a, void *fn, int c, void *d, int e, int f);
extern void func_02058048(void *a);
extern void func_0205ba64(int a, void *fn);
extern void func_020602bc(void);
extern void func_02060310(void);

struct S8180 {
    int *ptr;       /* 0x00 */
    int f04;        /* 0x04 */
    int f08;        /* 0x08 */
    int f0c;        /* 0x0c */
    u16 f10;        /* 0x10 */
    u16 pad12;      /* 0x12 */
    int f14;        /* 0x14 */
    int pad18[8];   /* 0x18 */
    int f38;        /* 0x38 */
    int pad3c[38];  /* 0x3c */
    u16 fd4;        /* 0xd4 */
    u16 padd6;      /* 0xd6 */
};
extern struct S8180 data_020a8180;
extern int data_020a8160[];
extern char data_020a81bc;
extern char data_020a8760;

void func_0206002c(void)
{
    volatile int local;

    *(volatile int *)&data_020a8180.f08 = ~2;
    *(volatile int *)&data_020a8180.f0c = 0;
    local = 0;
    data_020a8180.f14 = 0;
    data_020a8180.ptr = data_020a8160;
    MultiStore32Bytes(local, data_020a8160, 0x20);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)data_020a8160, 0x20);
    data_020a8180.f10 = 0;
    data_020a8180.fd4 = 0;
    data_020a8180.f38 = 4;
    func_02058200(&data_020a81bc, (void *)func_020602bc, 0, &data_020a8760, 0x400,
                  data_020a8180.f38);
    func_02058048(&data_020a81bc);
    func_0205ba64(0xb, (void *)func_02060310);
}
