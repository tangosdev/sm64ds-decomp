typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern void SetSubBg0Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
extern void SetSubBg2Offset(int a, int b);
extern void SetSubBg3Offset(int a, int b);
extern void _ZN3GXS15SetGraphicsModeEi(int m);
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, void* dst);
extern void Deallocate(void* p);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern u32 func_02054e88(void);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void func_02056434(const void* src, int offset, int count);
extern int GetOwnerLanguage(void);
extern char* _ZN3G2S13GetBG3CharPtrEv(void);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN2GX22SetBankForSubBGExtPlttEt(u16 a);
extern void _ZN3GXS18BeginLoadBGExtPlttEv(void);
extern void _ZN3GXS13LoadBGExtPlttEPKvjj(const void* p, u32 a, u32 b);
extern void _ZN3GXS16EndLoadBGExtPlttEv(void);
extern void func_020562b4(const void* src, u32 offset, u32 count);
extern void func_02056374(const void* src, u32 offset, u32 count);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern void* _ZN6Memory13operator_new2Ej(u32 sz);
extern void func_ov002_020f5ad4(char* c);

extern u8 BOTTOM_SCREEN_RELATED;

void func_ov002_020f5fe4(char* c)
{
    int f;
    int r5;

    SetSubBg0Offset(0, 0);
    SetSubBg1Offset(0, 0);
    SetSubBg2Offset(0, 0);
    SetSubBg3Offset(0, 0);
    _ZN3GXS15SetGraphicsModeEi(0);

    BOTTOM_SCREEN_RELATED &= ~0x1f;

    f = LoadFile(0x267);
    DecompressLZ16(f, (void*)0x6400000);
    Deallocate((void*)f);

    f = LoadFile(0x268);
    _ZN2GX11LoadOBJPlttEPKvjj((const void*)f, 0, 0x200);
    Deallocate((void*)f);

    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & ~3);
    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & 0x43) | 0x1100;
    *(volatile u16*)0x4001008 = (*(volatile u16*)0x4001008 & ~3);
    *(volatile u16*)0x4001008 = (*(volatile u16*)0x4001008 & 0x43) | 0xe08;
    *(volatile u16*)0x400100a = (*(volatile u16*)0x400100a & ~3) | 2;
    *(volatile u16*)0x400100a = (*(volatile u16*)0x400100a & 0x43) | 0x800;

    f = LoadFile(0x24f);
    DecompressLZ16(f, (void*)func_02054e88());
    Deallocate((void*)f);

    f = LoadFile(0x250);
    _ZN2GX10LoadBGPlttEPKvjj((const void*)f, 0, 2);
    _ZN3GXS10LoadBGPlttEPKvjj((const void*)f, 0, 0x200);
    Deallocate((void*)f);

    f = LoadFile(0x254);
    func_02056434((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & ~3) | 2;
    *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & 0x43) | 0xc90;

    if (GetOwnerLanguage() == 5) {
        r5 = LoadFile(0x271);
    } else if (GetOwnerLanguage() == 4) {
        r5 = LoadFile(0x26f);
    } else if (GetOwnerLanguage() == 3) {
        r5 = LoadFile(0x26d);
    } else if (GetOwnerLanguage() == 2) {
        r5 = LoadFile(0x26b);
    } else {
        r5 = LoadFile(0x251);
    }

    DecompressLZ16(r5, (void*)_ZN3G2S13GetBG3CharPtrEv());
    Deallocate((void*)r5);

    r5 = LoadFile(0x252);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)r5, 0x200);
    _ZN2GX22SetBankForSubBGExtPlttEt(0x80);
    _ZN3GXS18BeginLoadBGExtPlttEv();
    _ZN3GXS13LoadBGExtPlttEPKvjj((const void*)r5, 0x4000, 0x200);
    _ZN3GXS13LoadBGExtPlttEPKvjj((const void*)r5, 0x6000, 0x200);
    _ZN3GXS16EndLoadBGExtPlttEv();
    Deallocate((void*)r5);

    f = LoadFile(0x253);
    func_020562b4((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & ~3) | 2;
    *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & 0x43) | 0xa90;

    f = LoadFile(0x255);
    func_02056374((const void*)f, 0, 0x800);
    Deallocate((void*)f);

    BOTTOM_SCREEN_RELATED |= 0x10;

    {
        int f6 = LoadFile(0x256);
        r5 = LoadFile(0x257);
        DecompressLZ16(f6, (void*)0x6600000);
        _ZN3GXS11LoadOBJPlttEPKvjj((const void*)r5, 0, 0x200);
        Deallocate((void*)f6);
        Deallocate((void*)r5);
    }

    *(int*)(c + 0x510) = 0;
    if (*(int*)(c + 0x510) == 0) {
        *(int*)(c + 0x510) = (int)_ZN6Memory13operator_new2Ej(0x4000);
    }
    f = LoadFile(0x22c);
    DecompressLZ16(f, *(void**)(c + 0x510));
    Deallocate((void*)f);

    func_ov002_020f5ad4(c);
}
