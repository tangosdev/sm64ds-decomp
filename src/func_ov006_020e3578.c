typedef unsigned int u32;
typedef unsigned short u16;

extern char* func_020adc74(void* p);
extern void* _ZN2G213GetBG2CharPtrEv(void);
extern void DecompressLZ16(const void* src, void* dst);
extern void* LoadFile(int handle);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void Deallocate(void* ptr);
extern void func_020563d4(const void* src, u32 off, u32 cnt);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern unsigned func_02054de8(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void func_02056374(const void* src, u32 off, u32 cnt);
extern void func_ov004_020adc5c(void* p);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern void func_ov006_020e3388(char* self);
extern void func_ov006_020e3378(char* p);
extern void func_ov006_020e3250(char* c);
extern void func_ov006_020e2dbc(char* c);
extern void func_ov006_020e13a4(char* c);
extern void func_ov004_020b04d0(int v);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile u16* p, u16 a, u16 b, u16 c, u16 d);
extern int func_ov004_020adc1c(void);

extern unsigned char TOP_SCREEN_RELATED;
extern unsigned char BOTTOM_SCREEN_RELATED;
extern char data_ov006_0213c394;
extern char data_ov006_0213c3b4;

int func_ov006_020e3578(char* self)
{
    char* a = func_020adc74(&data_ov006_0213c394);
    char* b = func_020adc74(&data_ov006_0213c3b4);
    void* f;

    if (a == 0 || b == 0) return 0;

    TOP_SCREEN_RELATED |= 4;
    *(volatile u16*)0x400000c = (*(volatile u16*)0x400000c & ~3) | 2;
    *(volatile u16*)0x400000c = (*(volatile u16*)0x400000c & 0x43) | 0x1220;
    DecompressLZ16(a, _ZN2G213GetBG2CharPtrEv());

    f = LoadFile(0x2f);
    _ZN2GX10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(0x30);
    func_020563d4(f, 0, 0x800);
    Deallocate(f);

    {
        void* c7 = LoadFile(0xc7);
        void* c8 = LoadFile(0xc8);
        DecompressLZ16(c7, (void*)0x6400000);
        _ZN2GX11LoadOBJPlttEPKvjj(c8, 0, 0x100);

        BOTTOM_SCREEN_RELATED |= 4;
        *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & ~3) | 2;
        *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & 0x43) | 0x814;
        DecompressLZ16(b, (void*)func_02054de8());

        f = LoadFile(0x33);
        _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
        Deallocate(f);

        f = LoadFile(0x34);
        func_02056374(f, 0, 0x800);
        Deallocate(f);

        func_ov004_020adc5c(a);
        func_ov004_020adc5c(b);

        DecompressLZ16(c7, (void*)0x6600000);
        _ZN3GXS11LoadOBJPlttEPKvjj(c8, 0, 0x100);
        Deallocate(c7);
        Deallocate(c8);
    }

    func_ov006_020e3388(self);
    func_ov006_020e3378(self);
    func_ov006_020e3250(self);
    *(unsigned char*)(self + 0x4ee7) = 1;
    *(u16*)(self + 0x4ee0) = 0;
    func_ov006_020e2dbc(self);
    func_ov006_020e13a4(self);
    *(int*)(self + 0x4eac) = 1;
    func_ov004_020b04d0(0x20);
    *(u16*)(self + 0x4ee2) = 0x40;
    *(int*)(self + 0xa4) = 1;
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile u16*)0x4000050, 0, 0xd, 2, 0x10);
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile u16*)0x4001050, 0, 4, 2, 0x10);
    *(int*)(self + 0x4ed8) = func_ov004_020adc1c();
    return 1;
}
