//cpp
typedef short s16;
typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;

extern "C" {

extern void func_ov006_0212231c(void *p);
extern void _ZN3G3X6SetFogEbiii(int a, int b, int c, int d);
extern void InitialiseVramGlobals(void);
extern s32 _ZN4cstd4fdivEii(s32 a, s32 b);
extern void func_ov006_020c0134(void *cam);
extern int func_ov006_020cd658(unsigned char *a, int b);
extern void func_ov006_02120d8c(void *a, int b);
extern void func_ov006_020d0b2c(void);
extern void func_ov004_020b04d0(int v);

extern u8 data_0209d45c;
extern s16 data_02082414;

}

struct Obj {
    virtual void m00();
    virtual void m04();
    virtual void m08();
    virtual void m0c();
    virtual void m10();
    virtual void m14();
    virtual void m18();
    virtual void m1c();
    virtual void m20();
    virtual void m24();
    virtual void m28();
    virtual void m2c();
    virtual void m30();
    virtual void m34();
    virtual void m38();
    virtual void m3c();
    virtual void m40();
    virtual void m44();
    virtual void m48(int a);
};

extern "C" int func_ov006_02122198(char *base)
{
    s32 fov;

    *(s32 *)(base + 0x5d94) = 0x20;
    *(s32 *)(base + 0x5d98) = *(s32 *)(base + 0x5d94);
    func_ov006_0212231c(base);
    data_0209d45c = 0x1d;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    *(u16 *)0x4000060 = (*(u16 *)0x4000060 & ~0x3000) | 8;
    InitialiseVramGlobals();
    *(u16 *)0x4000008 = (*(u16 *)0x4000008 & ~3) | 1;
    fov = _ZN4cstd4fdivEii(0xc0000, (s32)data_02082414);
    *(s32 *)(base + 0x470c) = 0;
    *(s32 *)(base + 0x4710) = -0x64000;
    *(s32 *)(base + 0x4714) = 0;
    *(s32 *)(base + 0x4718) = 0;
    *(s32 *)(base + 0x471c) = 0;
    *(s32 *)(base + 0x4720) = fov;
    *(u16 *)(base + 0x4724) = 0x400;
    func_ov006_020c0134(base + 0x466c);
    *(s32 *)(base + 0x47c8) = 0;
    *(s32 *)(base + 0x47cc) = 0x82000;
    *(s32 *)(base + 0x47d0) = 0;
    *(s32 *)(base + 0x47d4) = 0;
    *(s32 *)(base + 0x47d8) = 0;
    *(s32 *)(base + 0x47dc) = fov;
    *(u16 *)(base + 0x47e0) = 0x400;
    func_ov006_020c0134(base + 0x4728);
    if (func_ov006_020cd658((unsigned char *)(base + 0x500c), 4) == 0)
        return 0;
    func_ov006_02120d8c(base + 0x5cd0, 5);
    func_ov006_020d0b2c();
    func_ov004_020b04d0(0x10);
    ((Obj *)base)->m48(-1);
    return 1;
}