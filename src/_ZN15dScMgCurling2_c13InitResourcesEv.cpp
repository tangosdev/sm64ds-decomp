//cpp
// @symbol _ZN15dScMgCurling2_c13InitResourcesEv
// recovered name: dScMgCurling2_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgCurling2_c.h"
/* dScMgCurling2_c::InitResources - recovered from vtable slot identity */
extern "C" {
char* func_ov004_020adc74(void* p);
void DecompressLZ16(const void* src, void* dst);
void* LoadFile(int handle);
void _ZN2GX10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
char* _ZN2G212GetBG2ScrPtrEv(void);
void MultiStore16(u16 val, char* dst, int nbytes);
void func_02056554(const void* src, int offset, int count);
char* _ZN2G212GetBG0ScrPtrEv(void);
void _ZN2GX11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
unsigned _ZN3G2S13GetBG2CharPtrEv(void);
void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
void Ov004_Deallocate(void* p);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, u32 a, u32 b);
void func_ov006_020e668c(char* c);
void func_ov006_020e6528(char* c);
void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16* p, u16 a, u16 b, u16 c, u16 d);
int func_ov004_020adc1c(void);

extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
}

s32 dScMgCurling2_c::InitResources()
{
    char *self = (char *)this;
    char* r6 = func_ov004_020adc74(&data_ov006_0213c5a0);
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
    DecompressLZ16(r6, (void*)_ZN3G2S13GetBG2CharPtrEv());

    f = LoadFile(0x45);
    _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Deallocate(f);

    f = LoadFile(0x43);
    func_02056374(f, 0, 0x800);
    Deallocate(f);

    Ov004_Deallocate(r6);

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
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16*)0x4000050, 0, 0xd, 2, 0x10);
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16*)0x4001050, 0, 4, 2, 0x10);
    *(int*)(self + 0x55ac) = func_ov004_020adc1c();
    return 1;
}
