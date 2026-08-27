#include "types.h"
extern void func_ov004_020b290c(void);
extern void func_ov004_020b2980(void);
extern void _ZN2GX12SetBankForBGEt(u16 a);
extern void _ZN2GX13SetBankForOBJEt(u16 a);
extern void _ZN2GX13SetBankForTexEt(u16 a);
extern void _ZN2GX17SetBankForTexPlttEt(u16 a);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN2GX15SetBankForSubBGEt(u16 a);
extern void _ZN2GX16SetBankForSubOBJEt(u16 a);
extern s32 GetGameLanguage(void);
extern void *func_ov004_020adc68(int id);
extern void DecompressLZ16(void *src, void *dst);
extern void Ov004_Deallocate(void *p);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *p, u32 len);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_ov004_020b0d30(void);
extern void _ZN3G3X6SetFogEbiii(int a, int b, int c, int d);
extern void InitialiseVramGlobals(void);
extern void FreeGfxSlotsById(int arg);

extern int data_ov004_020beb6c;
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern int data_ov006_0213e42c[];
extern int data_0208ee44;
extern int data_ov000_020beb74[];
extern void **data_0209d4a8;

void func_ov006_0210a708(char *obj)
{
    void *p;

    *(vu32 *)0x4001000u |= 0x10000u;
    data_ov004_020beb6c = 0;
    func_ov004_020b290c();
    func_ov004_020b2980();
    data_0209d45c = 0x10;
    data_0209d454 = 0x10;
    *(vu32 *)0x4000000u &= ~0x7000000u;
    *(vu32 *)0x4000000u &= ~0x38000000u;
    _ZN2GX12SetBankForBGEt(2);
    _ZN2GX13SetBankForOBJEt(0x10);
    _ZN2GX13SetBankForTexEt(1);
    _ZN2GX17SetBankForTexPlttEt(0x20);
    _ZN2GX15SetGraphicsModeEiii(1, 0, 1);
    *(vu32 *)0x4000000u &= 0xffcfffefu;
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);
    p = func_ov004_020adc68(data_ov006_0213e42c[GetGameLanguage()]);
    {
        char *dst = (char *)0x6400000; dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    {
        char *dst = (char *)0x6600000; dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    Ov004_Deallocate(p);
    p = func_ov004_020adc68(0xc3);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(p, 0x100u);
    _ZN2GX11LoadOBJPlttEPKvjj(p, 0x100u, 0x100u);
    _ZN3GXS11LoadOBJPlttEPKvjj(p, 0x100u, 0x100u);
    Ov004_Deallocate(p);
    func_ov004_020b0d30();
    data_0208ee44 = 1;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    InitialiseVramGlobals();
    FreeGfxSlotsById(0x1d);
    data_ov000_020beb74[1] = (int)obj;
    data_0209d4a8 = (void **)data_ov000_020beb74;
    *(vu32 *)0x40004ccu = 0x7fff;
    *(vu32 *)0x40004ccu = 0x40007fff;
}
