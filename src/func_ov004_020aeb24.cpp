//cpp
struct Obj {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28();
};

extern "C" {
extern void func_02012790(int a);
extern int func_ov004_020b8f78(char* p);
extern int _Z15ApproachLinear2Rsss(short* v, short a, short b);
extern void func_0203b958(short* o, short* a, short* b);
extern int _Z14ApproachLinearRiii(int* v, int a, int b);
extern void _ZN5Scene9SetFadersEP15FaderBrightness(void* f);
extern void func_0202ec9c(void* f, int a);
extern void _ZN5Scene14StartSceneFadeEjjt(unsigned a, unsigned b, unsigned short c);
extern void func_02012dd0(int a);
extern void func_ov004_020b9220(char* p);

extern unsigned char data_020a0e40[];
extern unsigned short data_020a0e5a[];
extern unsigned char TOUCH_INPUT_ARR[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];
extern char data_0209f61c[];

void func_ov004_020aeb24(char* c)
{
    unsigned char idx = data_020a0e40[0];
    unsigned short flags = data_020a0e5a[idx * 2];
    int unlocked;
    short tmp[2];
    short d0[2];
    short d1[2];
    short d2[2];
    short a, b;

    if ((flags & 0x40) || (flags & 0x80) || (flags & 0x20) || (flags & 0x10)) {
        func_02012790(0xe);
    }
    if (func_ov004_020b8f78(c + 0xf4) != 0) return;

    _Z15ApproachLinear2Rsss((short*)(c + 0x4634), 0x80, 0x10);
    _Z15ApproachLinear2Rsss((short*)(c + 0x4638), 0x80, 0x10);
    _Z15ApproachLinear2Rsss((short*)(c + 0x463e), 0x90, 5);

    idx = data_020a0e40[0];
    unlocked = 0;
    if (TOUCH_INPUT_ARR[idx * 4] != 0) {
        unlocked = (data_020a0de9[idx * 4] != 0);
    }
    if (unlocked == 0) goto after;
    if (*(short*)(c + 0x4646) >= 0) goto after;
    tmp[0] = data_020a0dea[idx * 4];
    tmp[1] = data_020a0deb[idx * 4];
    func_0203b958(d0, tmp, (short*)(c + 0x4634));
    func_0203b958(d1, tmp, (short*)(c + 0x4638));
    func_0203b958(d2, tmp, (short*)(c + 0x463c));
    a = d0[0]; if (a < 0) a = -a;
    if (a < 0x60) {
        b = d0[1]; if (b < 0) b = -b;
        if (b < 0x18) {
            func_02012790(0x62);
            *(short*)(c + 0x4646) = 0;
            *(short*)(c + 0x4644) = 0x10;
            goto after;
        }
    }
    a = d1[0]; if (a < 0) a = -a;
    if (a < 0x40) {
        b = d1[1]; if (b < 0) b = -b;
        if (b < 0x13) {
            func_02012790(0x63);
            *(short*)(c + 0x4646) = 1;
            *(short*)(c + 0x4644) = 0;
            goto after;
        }
    }
    a = d2[0]; if (a < 0) a = -a;
    if (a < 0x40) {
        b = d2[1]; if (b < 0) b = -b;
        if (b < 0x13) {
            func_02012790(0x62);
            *(short*)(c + 0x4646) = 2;
            *(short*)(c + 0x4644) = 0;
        }
    }

after:
    if (*(short*)(c + 0x4646) < 0) return;
    if (_Z14ApproachLinearRiii((int*)(c + 0x4640), 0x10000, 0x1000) == 0) return;
    if (_Z15ApproachLinear2Rsss((short*)(c + 0x4644), 0, 1) == 0) return;

    switch (*(short*)(c + 0x4646)) {
    case 0:
        ((Obj*)c)->v28();
        return;
    case 1:
        _ZN5Scene9SetFadersEP15FaderBrightness(data_0209f61c);
        func_0202ec9c(data_0209f61c, 1);
        _ZN5Scene14StartSceneFadeEjjt(5, 0, 0);
        if (*(int*)(c + 0x4648) != 0) return;
        func_02012dd0(0x3c);
        *(int*)(c + 0x4648) = 1;
        return;
    case 2:
        func_ov004_020b9220(c + 0xf4);
        *(short*)(c + 0x4646) = -1;
        *(int*)(c + 0x4640) = 0;
        return;
    default:
        ((Obj*)c)->v28();
        return;
    }
}
}
