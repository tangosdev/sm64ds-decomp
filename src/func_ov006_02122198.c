//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

struct Base {
    virtual int f00();
    virtual int f01();
    virtual int f02();
    virtual int f03();
    virtual int f04();
    virtual int f05();
    virtual int f06();
    virtual int f07();
    virtual int f08();
    virtual int f09();
    virtual int f10();
    virtual int f11();
    virtual int f12();
    virtual int f13();
    virtual int f14();
    virtual int f15();
    virtual int f16();
    virtual int f17();
    virtual int f18(int);   /* vtable offset 0x48 */
};

extern "C" {

extern u8 data_0209d45c;
extern s16 data_02082414;

extern void func_ov006_0212231c(void* arg0);
extern void G3X_SetFog(int, int, int, int);
extern void InitialiseVramGlobals(void);
extern int cstd_fdiv(int, int);
extern void func_ov006_020c0134(void* self);
extern int func_ov006_020cd658(void* a0, int a1);
extern void func_ov006_02120d8c(void* a, int b);
extern void func_ov006_020d0b2c(void);
extern void func_ov004_020b04d0(int v);

int func_ov006_02122198(void* self) {
    char* s = (char*)self;
    int q;

    *(int*)(s + 0x5d94) = 0x20;
    *(int*)(s + 0x5d98) = *(int*)(s + 0x5d94);
    func_ov006_0212231c(self);

    data_0209d45c = 0x1d;
    G3X_SetFog(0, 0, 2, 0x1000);

    *(volatile u16*)0x4000060 = (*(volatile u16*)0x4000060 & ~0x3000) | 8;
    InitialiseVramGlobals();

    *(volatile u16*)0x4000008 = (*(volatile u16*)0x4000008 & ~3) | 1;
    q = cstd_fdiv(0xc0000, data_02082414);

    *(int*)(s + 0x470c) = 0;
    *(int*)(s + 0x4710) = -0x64000;
    *(int*)(s + 0x4714) = 0;
    *(int*)(s + 0x4718) = 0;
    *(int*)(s + 0x471c) = 0;
    *(int*)(s + 0x4720) = q;
    *(s16*)(s + 0x4724) = 0x400;
    func_ov006_020c0134(s + 0x466c);

    *(int*)(s + 0x47c8) = 0;
    *(int*)(s + 0x47cc) = 0x82000;
    *(int*)(s + 0x47d0) = 0;
    *(int*)(s + 0x47d4) = 0;
    *(int*)(s + 0x47d8) = 0;
    *(int*)(s + 0x47dc) = q;
    *(s16*)(s + 0x47e0) = 0x400;
    func_ov006_020c0134(s + 0x4728);

    if (func_ov006_020cd658(s + 0x500c, 4) == 0) {
        return 0;
    }

    func_ov006_02120d8c(s + 0x5cd0, 5);
    func_ov006_020d0b2c();
    func_ov004_020b04d0(0x10);

    ((Base*)self)->f18(-1);
    return 1;
}

}
