typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef volatile unsigned int vu32;
typedef volatile unsigned short vu16;

extern void *_ZN2G213GetBG2CharPtrEv(void);
extern unsigned func_02054de8(void);
extern void *func_02054fb0(void);
extern void *_ZN3G2S12GetBG3ScrPtrEv(void);
extern u32 LoadCompressedFileAt(u16 fileID, void *target);

extern void *func_020adc74(void *p);
extern void func_ov004_020adc5c(void *p);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *p, u32 sz);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void DecompressLZ16(void *src, void *dst);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3G3X6SetFogEbiii(int enable, int a, int b, int c);
extern void InitialiseVramGlobals(void);
extern void _ZN5Model17UpdateFileOffsetsER8BMD_File(void *file);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thisPtr, void *file, int a, int b);
extern void func_ov006_021279b0(void *p);
extern void func_ov006_02126ee4(void *p);
extern void func_ov006_02126a98(void *p);

extern int data_0208ee44;
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern char data_ov006_0214009c;
extern char data_ov006_021400bc;
extern char data_ov006_021400dc;
extern char data_ov006_021400fc;

int func_ov006_02129268(void *arg0)
{
    u8 *r4 = (u8 *)arg0;
    void *buf;

    data_0208ee44 = 1;
    *(vu16 *)0x4000304 &= ~0x8000;
    *(r4 + 0xb9f8) = 1;

    *(vu16 *)0x400000c = (*(vu16 *)0x400000c & 0x43) | 0x5214;
    *(vu16 *)0x400000c &= ~0x40;
    *(vu32 *)0x4000018 = 0;
    *(vu16 *)0x400000c = (*(vu16 *)0x400000c & ~3) | 2;

    *(vu16 *)0x400100c = (*(vu16 *)0x400100c & 0x43) | 0x5214;
    *(vu16 *)0x400100c &= ~0x40;
    *(vu32 *)0x4001018 = 0;
    *(vu16 *)0x400100c = (*(vu16 *)0x400100c & ~3) | 2;

    *(vu16 *)0x400000e = (*(vu16 *)0x400000e & 0x43) | 0x1614;
    *(vu16 *)0x400000e &= ~0x40;
    *(vu32 *)0x400001c = 0;
    *(vu16 *)0x400000e = (*(vu16 *)0x400000e & ~3) | 3;

    *(vu16 *)0x400100e = (*(vu16 *)0x400100e & 0x43) | 0x1614;
    *(vu16 *)0x400100e &= ~0x40;
    *(vu32 *)0x400101c = 0;
    *(vu16 *)0x400100e = (*(vu16 *)0x400100e & ~3) | 3;

    LoadCompressedFileAt(0xad, (u8 *)_ZN2G213GetBG2CharPtrEv() + 0x4000);
    LoadCompressedFileAt(0xad, (void *)(func_02054de8() + 0x4000));
    LoadCompressedFileAt(0xac, func_02054fb0());
    LoadCompressedFileAt(0xac, _ZN3G2S12GetBG3ScrPtrEv());

    data_0209d45c |= 0xc;
    data_0209d454 |= 0xc;

    buf = func_020adc74(&data_ov006_0214009c);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(buf, 0x100);
    _ZN2GX10LoadBGPlttEPKvjj(buf, 0x100, 0x100);
    _ZN3GXS10LoadBGPlttEPKvjj(buf, 0x100, 0x100);
    func_ov004_020adc5c(buf);

    buf = func_020adc74(&data_ov006_021400bc);
    DecompressLZ16(buf, (void *)0x6400000);
    DecompressLZ16(buf, (void *)0x6600000);
    func_ov004_020adc5c(buf);

    buf = func_020adc74(&data_ov006_021400dc);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(buf, 0x100);
    _ZN2GX11LoadOBJPlttEPKvjj(buf, 0, 0x100);
    _ZN3GXS11LoadOBJPlttEPKvjj(buf, 0, 0x100);
    func_ov004_020adc5c(buf);

    *(vu16 *)0x4000008 = (*(vu16 *)0x4000008 & ~3) | 1;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    *(vu16 *)0x4000060 = (*(vu16 *)0x4000060 & ~0x3000) | 8;
    InitialiseVramGlobals();

    *(void **)(r4 + 0xabf4) = func_020adc74(&data_ov006_021400fc);
    _ZN5Model17UpdateFileOffsetsER8BMD_File(*(void **)(r4 + 0xabf4));

    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(r4 + 0xaba4, *(void **)(r4 + 0xabf4), 1, -1) == 0)
        return 0;

    *(vu16 *)0x4000008 = (*(vu16 *)0x4000008 & ~3) | 1;
    data_0209d45c |= 1;

    func_ov006_021279b0(arg0);
    func_ov006_02126ee4(arg0);
    func_ov006_02126a98(arg0);
    return 1;
}
