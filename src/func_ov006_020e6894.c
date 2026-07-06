typedef unsigned int u32;
typedef unsigned short u16;

extern char* func_020adc74(void* p);
extern int func_02054d88(void);
extern void DecompressLZ16(const void* src, void* dst);
extern void* LoadFile(int handle);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void Deallocate(void* ptr);
extern void func_02056314(void* dst, u32 offset, u32 len);
extern char* _ZN2G212GetBG2ScrPtrEv(void);
extern void MultiStore16(u16 val, char* dst, int nbytes);
extern void func_020563d4(const void* src, u32 off, u32 cnt);
extern void func_02056554(const void* src, int offset, int count);
extern char* _ZN2G212GetBG0ScrPtrEv(void);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern unsigned func_02054de8(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
extern void func_02056374(const void* src, u32 off, u32 cnt);
extern void func_ov004_020adc5c(void* p);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
extern void func_ov006_020e668c(char* c);
extern void func_ov006_020e667c(char* p);
extern void func_ov006_020e6528(char* c);
extern void func_ov006_020e5ffc(char* self);
extern void func_ov006_020e48d4(char* c);
extern void func_ov004_020b04d0(int v);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile u16* p, u16 a, u16 b, u16 c, u16 d);
extern int func_ov004_020adc1c(void);

extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
extern char data_ov006_0213c5a0;

int func_ov006_020e6894(char* self)
{
    char* r6 = func_020adc74(&data_ov006_0213c5a0);
    void* f;
    void* c7;
    void* c8;
    volatile u16 sp4;
    volatile u16 sp6;

    if (r6 == 0) return 0;

    data_0209d45c |= 8;
    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & ~3) | 2;
    *(volatile u16*)0x400000e = (*(volatile u16*)0x400000e & 0x43) | 0x1220;
    DecompressLZ16(r6, (void*)func_02054d88());

    f = LoadFile(0x45);
    _ZN2GX10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(0x42);
    func_02056314(f, 0, 0x800);
    Deallocate(f);

    data_0209d45c |= 4;
    *(volatile u16*)0x400000c = (*(volatile u16*)0x400000c & ~3) | 2;
    *(volatile u16*)0x400000c = (*(volatile u16*)0x400000c & 0x43) | 0x5420;

    f = LoadFile(0x41);
    {
        char* b = _ZN2G212GetBG2ScrPtrEv();
        sp4 = 0x4300;
        MultiStore16(sp4, b, 0x1000);
    }
    func_020563d4(f, 0, 0x800);

    *(volatile u16*)0x4000008 = (*(volatile u16*)0x4000008 & ~3) | 2;
    *(volatile u16*)0x4000008 = (*(volatile u16*)0x4000008 & 0x43) | 0x5620;
    {
        char* b = _ZN2G212GetBG0ScrPtrEv();
        sp6 = 0x4300;
        MultiStore16(sp6, b, 0x1000);
    }
    func_02056554(f, 0, 0x800);
    Deallocate(f);

    c7 = LoadFile(0xc7);
    c8 = LoadFile(0xc8);
    DecompressLZ16(c7, (void*)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj(c8, 0, 0x100);

    data_0209d454 |= 4;
    *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & ~3) | 2;
    *(volatile u16*)0x400100c = (*(volatile u16*)0x400100c & 0x43) | 0x814;
    DecompressLZ16(r6, (void*)func_02054de8());

    f = LoadFile(0x45);
    _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(0x43);
    func_02056374(f, 0, 0x800);
    Deallocate(f);

    func_ov004_020adc5c(r6);

    DecompressLZ16(c7, (void*)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj(c8, 0, 0x100);
    Deallocate(c7);
    Deallocate(c8);

    func_ov006_020e668c(self);
    func_ov006_020e667c(self);
    func_ov006_020e6528(self);
    *(unsigned char*)(self + 0x55bb) = 1;
    *(u16*)(self + 0x55b4) = 0;
    *(unsigned char*)(self + 0x55c3) = 1;
    func_ov006_020e5ffc(self);
    func_ov006_020e48d4(self);
    *(int*)(self + 0x5580) = 1;
    func_ov004_020b04d0(0x20);
    *(u16*)(self + 0x55b6) = 0x40;
    *(int*)(self + 0xa4) = 1;
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile u16*)0x4000050, 0, 0xd, 2, 0x10);
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile u16*)0x4001050, 0, 4, 2, 0x10);
    *(int*)(self + 0x55ac) = func_ov004_020adc1c();
    return 1;
}
