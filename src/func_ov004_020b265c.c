typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;
typedef volatile unsigned int vu32;
typedef volatile unsigned short vu16;

extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3GXS15SetGraphicsModeEi(int a);
extern void func_ov004_020b2980(void);
extern void func_ov004_020b290c(void);
extern void _ZN2GX12SetBankForBGEt(u16 a);
extern void _ZN2GX13SetBankForOBJEt(u16 a);
extern void _ZN2GX15SetBankForSubBGEt(u16 a);
extern void _ZN2GX16SetBankForSubOBJEt(u16 a);
extern s32 func_ov004_020ad674(void);
extern void *func_ov004_020adc68(int id);
extern void DecompressLZ16(void *src, void *dst);
extern void func_ov004_020adc5c(void *p);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *p, u32 len);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void func_ov004_020b0d30(void);
extern void func_ov004_020b0aa0(int arg);

extern int data_ov004_020beb6c;
extern u8 TOP_SCREEN_RELATED;
extern int data_ov004_020bbfe4[];
extern int data_ov004_020beb74[];
extern u8 BOTTOM_SCREEN_RELATED;
extern void **SCENE_RELATED;
extern int GAME_SPEED_RELATED;

void func_ov004_020b265c(char *obj)
{
    void *p;

    data_ov004_020beb6c = 0;
    obj[0x68] = 0;
    *(int *)(obj + 0x6c) = -1;
    *(vu32 *)0x4001000u |= 0x10000u;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);
    _ZN3GXS15SetGraphicsModeEi(0);
    *(vu32 *)0x4001000u &= 0xffcfffefu;
    *(vu16 *)0x4000304u |= 0x8000u;
    func_ov004_020b2980();
    func_ov004_020b290c();
    *(vu32 *)0x4000000u &= ~0x7000000u;
    *(vu32 *)0x4000000u &= ~0x38000000u;
    _ZN2GX12SetBankForBGEt(3);
    _ZN2GX13SetBankForOBJEt(0x10);
    TOP_SCREEN_RELATED = 0x10;
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);
    p = func_ov004_020adc68(data_ov004_020bbfe4[func_ov004_020ad674()]);
    {
        char *dst = (char *)0x6400000; dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    {
        char *dst = (char *)0x6600000; dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    func_ov004_020adc5c(p);
    p = func_ov004_020adc68(0xc3);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(p, 0x100u);
    _ZN2GX11LoadOBJPlttEPKvjj(p, 0x100u, 0x100u);
    _ZN3GXS11LoadOBJPlttEPKvjj(p, 0x100u, 0x100u);
    func_ov004_020adc5c(p);
    BOTTOM_SCREEN_RELATED = 0x10;
    data_ov004_020beb74[1] = (int)obj;
    SCENE_RELATED = (void **)data_ov004_020beb74;
    func_ov004_020b0d30();
    func_ov004_020b0aa0(0x1d);
    GAME_SPEED_RELATED = 1;
}
