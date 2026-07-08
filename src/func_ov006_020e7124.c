typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef volatile unsigned int vu32;

extern void func_ov004_020b290c(void);
extern void func_ov004_020b2980(void);
extern void _ZN2GX15DisableAllBanksEv(void);
extern void _ZN2GX13SetBankForTexEt(u16 a);
extern void _ZN2GX17SetBankForTexPlttEt(u16 a);
extern void func_02054748(int x);
extern void _ZN2GX13SetBankForOBJEt(u16 a);
extern void _ZN2GX12SetBankForBGEt(u16 a);
extern int func_ov004_020ad674(void);
extern void *func_ov004_020adc68(int id);
extern void DecompressLZ16(void *src, void *dst);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void InitialiseVramGlobals(void);
extern void func_ov004_020b0d30(void);
extern void func_ov006_020e7428(void);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3GXS15SetGraphicsModeEi(int a);

extern u8 TOP_SCREEN_RELATED;
extern u8 BOTTOM_SCREEN_RELATED;
extern u8 _ZN3OAM14subOAMDisabledE;
extern int data_ov006_0213c5fc[];
extern void *data_ov006_02141a4c;
extern void *data_ov006_02141a48;
extern int GAME_SPEED_RELATED;
extern void *data_ov006_02141a44;
extern void *data_ov006_02141a40;
extern void *data_ov006_02141a50;
extern int func_020beb74[];
extern void *SCENE_RELATED;

void func_ov006_020e7124(char *obj)
{
    void *p;

    *(vu32 *)0x4001000u |= 0x10000u;
    TOP_SCREEN_RELATED = 0x10;
    BOTTOM_SCREEN_RELATED = 0x10;
    func_ov004_020b290c();
    func_ov004_020b2980();
    _ZN3OAM14subOAMDisabledE = 1;
    _ZN2GX15DisableAllBanksEv();
    _ZN2GX13SetBankForTexEt(1);
    _ZN2GX17SetBankForTexPlttEt(0x40);
    *(vu32 *)0x4000000u &= ~0x7000000u;
    *(vu32 *)0x4000000u &= ~0x38000000u;
    func_02054748(0);
    _ZN2GX13SetBankForOBJEt(0x10);
    _ZN2GX12SetBankForBGEt(2);
    p = func_ov004_020adc68(data_ov006_0213c5fc[func_ov004_020ad674()]);
    data_ov006_02141a4c = p;
    {
        char *dst = (char *)0x6400000;
        dst += 0x4000;
        DecompressLZ16(p, dst);
    }
    p = func_ov004_020adc68(0xc3);
    data_ov006_02141a48 = p;
    _ZN2GX11LoadOBJPlttEPKvjj(p, 0x100u, 0x100u);
    _ZN3GXS11LoadOBJPlttEPKvjj(data_ov006_02141a48, 0x100u, 0x100u);
    InitialiseVramGlobals();
    func_ov004_020b0d30();
    func_ov006_020e7428();
    GAME_SPEED_RELATED = 1;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 1);
    _ZN3GXS15SetGraphicsModeEi(5);
    *(vu32 *)0x4000000u &= 0xffcfffefu;
    data_ov006_02141a44 = obj + 0x466c;
    data_ov006_02141a40 = obj + 0x466c;
    data_ov006_02141a50 = obj + 0x4728;
    func_020beb74[1] = (int)obj;
    SCENE_RELATED = (void *)func_020beb74;
}
