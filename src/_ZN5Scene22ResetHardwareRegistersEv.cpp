//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed int s32;

struct Matrix2x2 { int m[4]; };

extern "C" {
extern void _ZN2GX15DisableAllBanksEv(void);
extern int func_02053c10(int enable);
extern int func_02053be0(int enable);
extern void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c);
extern void _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii(volatile void* p, Matrix2x2* m, int a, int b, int c, int d);
extern void func_02053a90(u16* out, int v);
extern void _ZN3GXS15SetGraphicsModeEi(int a);
extern Matrix2x2 data_02092668;
}

struct Scene { void ResetHardwareRegisters(); };

void Scene::ResetHardwareRegisters()
{
    Matrix2x2 m;

    m = data_02092668;

    *(volatile u16*)0x4000304 |= 0x8000;
    *(volatile u16*)0x4000304 = (*(volatile u16*)0x4000304 & 0xfffffdf1) | 0x20e;
    *(volatile u16*)0x4000304 |= 1;

    _ZN2GX15DisableAllBanksEv();
    func_02053c10(0);
    func_02053be0(1);
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);

    *(volatile u32*)0x4000000 &= ~0x1f00;
    *(volatile u32*)0x4000000 &= ~0xe000;
    *(volatile u32*)0x4000000 &= ~0x38000000;
    *(volatile u32*)0x4000000 &= ~0x7000000;
    *(volatile u32*)0x4000000 &= 0xffcfffef;
    *(volatile u32*)0x4000000 &= 0xffbfff9f;
    *(volatile u32*)0x4000000 &= ~0x800000;
    *(volatile u32*)0x4000064 = 0x80000000;

    *(volatile u16*)0x4000008 &= 0x43;
    *(volatile u16*)0x400000a &= 0x43;
    *(volatile u16*)0x400000c &= 0x43;
    *(volatile u16*)0x400000e &= 0x43;
    *(volatile u16*)0x4000008 &= ~0x40;
    *(volatile u16*)0x400000a &= ~0x40;
    *(volatile u16*)0x400000c &= ~0x40;
    *(volatile u16*)0x400000e &= ~0x40;
    *(volatile u16*)0x4000008 &= ~3;
    *(volatile u16*)0x400000a &= ~3;
    *(volatile u16*)0x400000c &= ~3;
    *(volatile u16*)0x400000e &= ~3;

    *(volatile u32*)0x4000010 = 0;
    *(volatile u32*)0x4000014 = 0;
    *(volatile u32*)0x4000018 = 0;
    *(volatile u32*)0x400001c = 0;

    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile void*)0x4000020, &m, 0, 0, 0, 0);
    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile void*)0x4000030, &m, 0, 0, 0, 0);

    *(volatile u16*)0x4000048 &= ~0x3f;
    *(volatile u16*)0x4000048 &= ~0x3f00;
    *(volatile u16*)0x400004a &= ~0x3f;
    *(volatile u16*)0x400004a &= ~0x3f00;

    *(volatile u16*)0x4000040 = 0;
    *(volatile u16*)0x4000044 = 0;
    *(volatile u16*)0x4000042 = 0;
    *(volatile u16*)0x4000046 = 0;
    *(volatile u8*)0x400004c = 0;
    *(volatile u8*)0x400004d = 0;

    func_02053a90((u16*)0x400006c, 0);
    _ZN3GXS15SetGraphicsModeEi(0);

    *(volatile u32*)0x4001000 &= ~0x1f00;
    *(volatile u32*)0x4001000 &= ~0xe000;
    *(volatile u32*)0x4001000 &= 0xffcfffef;
    *(volatile u32*)0x4001000 &= 0xffbfff9f;
    *(volatile u32*)0x4001000 &= ~0x800000;

    *(volatile u16*)0x4001008 &= 0x43;
    *(volatile u16*)0x400100a &= 0x43;
    *(volatile u16*)0x400100c &= 0x43;
    *(volatile u16*)0x400100e &= 0x43;
    *(volatile u16*)0x4001008 &= ~0x40;
    *(volatile u16*)0x400100a &= ~0x40;
    *(volatile u16*)0x400100c &= ~0x40;
    *(volatile u16*)0x400100e &= ~0x40;
    *(volatile u16*)0x4001008 &= ~3;
    *(volatile u16*)0x400100a &= ~3;
    *(volatile u16*)0x400100c &= ~3;
    *(volatile u16*)0x400100e &= ~3;

    *(volatile u32*)0x4001010 = 0;
    *(volatile u32*)0x4001014 = 0;
    *(volatile u32*)0x4001018 = 0;
    *(volatile u32*)0x400101c = 0;

    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile void*)0x4001020, &m, 0, 0, 0, 0);
    _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii((volatile void*)0x4001030, &m, 0, 0, 0, 0);

    *(volatile u16*)0x4001048 &= ~0x3f;
    *(volatile u16*)0x4001048 &= ~0x3f00;
    *(volatile u16*)0x400104a &= ~0x3f;
    *(volatile u16*)0x400104a &= ~0x3f00;

    *(volatile u16*)0x4001040 = 0;
    *(volatile u16*)0x4001044 = 0;
    *(volatile u16*)0x4001042 = 0;
    *(volatile u16*)0x4001046 = 0;
    *(volatile u8*)0x400104c = 0;
    *(volatile u8*)0x400104d = 0;

    func_02053a90((u16*)0x400106c, 0);
}
