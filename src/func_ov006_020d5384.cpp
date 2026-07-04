//cpp
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef short s16;
typedef int s32;

struct Obj {
    virtual void v00();
    virtual void v04();
    virtual void v08();
    virtual void v0c();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual void v38();
    virtual void v3c();
    virtual void v40();
    virtual void v44();
    virtual void v48();
    virtual void v4c();
    virtual void v50();
    virtual void v54();
    virtual void v58();
    virtual void v5c();
    virtual void v60();
    virtual void v64();
    virtual void v68();
    virtual void v6c();
    virtual void v70();
    virtual void v74();
    virtual void v78();
    virtual void v7c();
    virtual void v80();
    virtual void v84();
    virtual void v88();
    virtual void v8c();
    virtual int v90();
};

extern "C" {
    int _ZN6Memory8AllocateEj(u32 size);
    void *func_02054efc(void);
    void MultiStore16(u16 val, void *dst, int nbytes);
    void *_ZN3G2S13GetBG0CharPtrEv(void);
    void func_ov004_020af2f8(void *self, int a, int b, int c);
    int func_02054d88(void);
    u32 LoadCompressedFileAt(u16 fileID, void *target);
    int LoadFile(int handle);
    void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
    void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
    void Deallocate(void *ptr);
    void *func_02054fb0(void);
    void *_ZN3G2S13GetBG3CharPtrEv(void);
    void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
    void *_ZN3G2S12GetBG3ScrPtrEv(void);
    void *_ZN3G2S12GetBG2ScrPtrEv(void);
    void _ZN3G2x13SetBlendAlphaEPVttttt(void *p, u16 a, u16 b, u16 c, u16 d);
    int GetOwnerLanguage(void);
    void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
    void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
    void func_ov006_020d3ba0(void *self);

    extern s32 data_0208ee44;
    extern u8 data_0209d45c;
    extern u8 data_0209d454;
}

extern "C" int func_ov006_020d5384(Obj *c)
{
    volatile u16 sp4;
    volatile u16 sp6;
    void *f;

    *(int *)((char *)c + 0x470c) = _ZN6Memory8AllocateEj(0x15800);
    *(int *)((char *)c + 0x4710) = _ZN6Memory8AllocateEj(0x15800);

    if (c->v90() != 0) {
        *(int *)((char *)c + 0x4700) = 0x78;
        *(int *)((char *)c + 0x53e4) = 2;
    } else {
        *(int *)((char *)c + 0x4700) = 0x98;
        *(int *)((char *)c + 0x53e4) = 2;
    }

    data_0208ee44 = 1;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x3218;
    *(volatile u16 *)0x4000008 &= ~0x40;
    *(volatile s32 *)0x4000010 = 0;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 1;

    f = func_02054efc();
    sp4 = 0;
    MultiStore16((u16)sp4, f, 0x6000);

    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x2214;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile s32 *)0x4001010 = 0;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & ~3) | 1;

    f = _ZN3G2S13GetBG0CharPtrEv();
    sp6 = 0;
    MultiStore16((u16)sp6, f, 0x6000);

    func_ov004_020af2f8(c, 1, 0, 2);

    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x140c;
    *(volatile u16 *)0x400000e &= ~0x40;
    *(volatile s32 *)0x400001c = 0;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 3;

    if (c->v90() != 0) {
        LoadCompressedFileAt(0x1f, (void *)func_02054d88());
        f = (void *)LoadFile(0x20);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)f, 0x1a0);
        _ZN2GX10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
        Deallocate(f);
        LoadCompressedFileAt(0x21, func_02054fb0());
    } else {
        LoadCompressedFileAt(0x15, (void *)func_02054d88());
        f = (void *)LoadFile(0x16);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)f, 0x1a0);
        _ZN2GX10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
        Deallocate(f);
        LoadCompressedFileAt(0x17, func_02054fb0());
    }

    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x40c;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile s32 *)0x4001018 = 0;
    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & 0x43) | 0x60c;
    *(volatile u16 *)0x400100e &= ~0x40;
    *(volatile s32 *)0x400101c = 0;
    *(volatile u16 *)0x400100e = (*(volatile u16 *)0x400100e & ~3) | 3;

    if (c->v90() != 0) {
        LoadCompressedFileAt(0x1c, _ZN3G2S13GetBG3CharPtrEv());
        f = (void *)LoadFile(0x1d);
        _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
        Deallocate(f);
        LoadCompressedFileAt(0x1e, _ZN3G2S12GetBG3ScrPtrEv());
        data_0209d45c |= 9;
        data_0209d454 |= 9;
    } else {
        LoadCompressedFileAt(0x1a, _ZN3G2S13GetBG3CharPtrEv());
        f = (void *)LoadFile(0x1b);
        _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
        Deallocate(f);
        LoadCompressedFileAt(0x19, _ZN3G2S12GetBG2ScrPtrEv());
        LoadCompressedFileAt(0x18, _ZN3G2S12GetBG3ScrPtrEv());
        *(volatile s16 *)0x4000050 = 0;
        _ZN3G2x13SetBlendAlphaEPVttttt((void *)0x4001050, 4, 8, 6, 0x10);
        data_0209d45c |= 9;
        data_0209d454 |= 0xd;
    }

    if (c->v90() != 0) {
        if (GetOwnerLanguage() == 5) {
            LoadCompressedFileAt(0xf, (void *)0x6400000);
            LoadCompressedFileAt(0xf, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 4) {
            LoadCompressedFileAt(0xc, (void *)0x6400000);
            LoadCompressedFileAt(0xc, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 3) {
            LoadCompressedFileAt(0xb, (void *)0x6400000);
            LoadCompressedFileAt(0xb, (void *)0x6600000);
        } else if (GetOwnerLanguage() == 2) {
            LoadCompressedFileAt(0xa, (void *)0x6400000);
            LoadCompressedFileAt(0xa, (void *)0x6600000);
        } else {
            LoadCompressedFileAt(0xd, (void *)0x6400000);
            LoadCompressedFileAt(0xd, (void *)0x6600000);
        }
        f = (void *)LoadFile(0xe);
        _ZN2GX11LoadOBJPlttEPKvjj(f, 0, 0x100);
        _ZN3GXS11LoadOBJPlttEPKvjj(f, 0, 0x100);
        Deallocate(f);
    } else {
        LoadCompressedFileAt(0x10, (void *)0x6400000);
        LoadCompressedFileAt(0x10, (void *)0x6600000);
        f = (void *)LoadFile(0x11);
        _ZN2GX11LoadOBJPlttEPKvjj(f, 0, 0x100);
        _ZN3GXS11LoadOBJPlttEPKvjj(f, 0, 0x100);
        Deallocate(f);
    }

    *(int *)((char *)c + 0x53d4) = 0;
    *(int *)((char *)c + 0x53e8) = *(int *)((char *)c + 0xbc) * 5;
    func_ov006_020d3ba0(c);
    return 1;
}
