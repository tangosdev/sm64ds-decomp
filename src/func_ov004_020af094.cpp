//cpp
typedef unsigned short u16;
typedef short s16;

extern "C" void func_02019028(void);
extern "C" void MultiCopy_Int(int *dst, int *src, int len);
extern "C" void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *addr, unsigned int size);
extern "C" void _ZN2GX10LoadBGPlttEPKvjj(const void *src, unsigned int offset, unsigned int size);
extern "C" void _ZN3GXS10LoadBGPlttEPKvjj(const void *src, unsigned int offset, unsigned int size);
extern "C" void MultiStore16(unsigned short val, char *dst, int nbytes);
extern "C" void func_0201f32c(int arg0);
extern "C" int func_ov004_020ad674(void);
extern "C" void DecompressLZ16(void *src, void *dst);

extern "C" unsigned char data_0209d45c;
extern "C" unsigned char data_0209d454;
extern "C" char data_ov004_020bea28[];
extern "C" char data_ov004_020beac8[];
extern "C" void *data_ov004_020bbf94[];

struct Base {
    virtual void d0();
    virtual void d1();
    virtual void fn2();
    virtual void fn3();
    virtual void fn4();
    virtual void fn5();
    virtual void fn6();
    virtual void fn7();
    virtual void fn8();
    virtual void fn9();
    virtual void fn10();
    virtual void fn11();
    virtual void fn12();
    virtual void fn13();
    virtual void fn14();
    virtual void fn15();
    virtual void fn16();
    virtual void fn17();
    virtual void fn18();
    virtual void fn19();
    virtual void fn20();
    virtual void fn21();
    virtual void fn22();
    virtual void fn23();
    virtual void fn24();
    virtual void fn25();
    virtual int Init();
};

struct Obj : Base {
    virtual int Init();
    char pad[0x4700];
};

extern "C" void func_ov004_020af094(Obj *self)
{
    char *c = (char *)self;

    if (self->Init() != 0)
        func_02019028();

    *(int *)(c + 0x224) = (*(volatile u16 *)0x4000304 & 0x8000) >> 15;

    *(int *)(c + 0x21c) = data_0209d45c;
    *(int *)(c + 0x220) = data_0209d454;

    *(volatile u16 *)0x4000304 |= 0x8000;
    data_0209d45c = 0;
    data_0209d454 = 0;

    *(volatile int *)0x4000000 &= ~0x1f00;
    *(volatile int *)0x4001000 &= ~0x1f00;
    *(volatile int *)0x4000000 &= ~0xe000;
    *(volatile int *)0x4001000 &= ~0xe000;

    MultiCopy_Int((int *)0x5000020, (int *)(c + 0x4228), 0x1e0);
    {
        int *ip4228 = (int *)(c + 0x4228);
        MultiCopy_Int((int *)0x5000400, ip4228 + 0x80, 0x80);
    }
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((void *)(c + 0x4228), 0x400);

    _ZN2GX10LoadBGPlttEPKvjj((const void *)data_ov004_020bea28, 0x20, 0xa0);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)data_ov004_020beac8, 0, 0x80);

    {
        volatile unsigned short tmp = 0;
        MultiStore16(tmp, (char *)0x5000000, 2);
    }
    {
        volatile unsigned short tmp = 0;
        MultiStore16(tmp, (char *)0x5000400, 2);
    }

    func_0201f32c(*(s16 *)(c + 0x465e));

    data_0209d45c = 0x12;
    data_0209d454 = 0x10;

    if (self->Init() == 2)
        return;

    int *vbase = (int *)0x6600000;
    int *vram = vbase + 0x1800;
    MultiCopy_Int(vram, (int *)(c + 0x2228), 0x2000);
    int idx = func_ov004_020ad674();
    DecompressLZ16(data_ov004_020bbf94[idx], vram);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((void *)(c + 0x2228), 0x2000);
}
