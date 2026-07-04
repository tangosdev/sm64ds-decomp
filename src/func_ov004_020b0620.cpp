//cpp
extern "C" {
int func_ov004_020b8ee0(char *p);
void func_ov004_020aeb24(char *c);
void func_ov004_020b8778(char *c);
void func_ov004_020b321c(char *c);
void func_ov004_020adf2c(char *c);
void _Z14ApproachLinearRiii(int *p, int a, int b);
}

struct Scene {
    virtual int f00();
    virtual int f01();
    virtual int f02();
    virtual int f03();
    virtual int f04();
    virtual int f05();
};

struct Self {
    virtual void v00();  virtual void v01();  virtual void v02();  virtual void v03();
    virtual void v04();  virtual void v05();  virtual void v06();  virtual void v07();
    virtual void v08();  virtual void v09();  virtual void v10();  virtual void v11();
    virtual void v12();  virtual void v13();  virtual void v14();  virtual void v15();
    virtual void v16();  virtual void v17();  virtual void v18();  virtual void v19();
    virtual void v20();  virtual void v21();  virtual void v22();  virtual void v23();
    virtual void v24();  virtual void v25();  virtual void v26();
    virtual void m6c();  /* index 27 = 0x6c */
    virtual void m70();  /* index 28 = 0x70 */
};

extern Scene *data_0209f5bc;
extern unsigned char data_020a0e40;
extern unsigned short data_020a0e5a[];
extern unsigned char data_020a0de8[];
extern char data_ov004_020bf648[];
extern char data_ov004_020bebe8[];
extern int data_0208ee44;

extern "C" int _ZN5Scene14BeforeBehaviorEv();

#pragma opt_strength_reduction off
extern "C" int func_ov004_020b0620(char *self)
{
    int mode;
    unsigned short flags;

    if (_ZN5Scene14BeforeBehaviorEv() == 0)
        return 0;

    if (((Scene *)data_0209f5bc)->f05()) {
        mode = data_020a0e40;
        flags = data_020a0e5a[mode * 2];
        if ((flags & 8) || (flags & 4) || (flags & 1) || (flags & 2)) {
            if (*(int *)(self + 0x4628) != 0) {
                if (*(short *)(self + 0x4646) < 0) {
                    if (func_ov004_020b8ee0(self + 0xf4))
                        ((Self *)self)->m70();
                }
            } else {
                if (data_020a0de8[mode * 4] == 0)
                    ((Self *)self)->m6c();
            }
        }
    }

    if (*(int *)(self + 0x4628) != 0) {
        func_ov004_020aeb24(self);
        return 0;
    }
    if (*(int *)(self + 0x462c) != 0)
        return 0;

    if (*(int *)(self + 0xf0) == 0) {
        int i;
        char *g;
        func_ov004_020b8778(self + 0xcc);
        g = data_ov004_020bf648;
        for (i = 0; i < 3; i++) {
            if (*(int *)(g + 0x20) != 0x1d)
                func_ov004_020b321c(g);
            g += 0x134;
        }
    }

    {
        int j;
        char *p = data_ov004_020bebe8;
        char *base = data_ov004_020bebe8;
        for (j = 0; j < 0x40; j++) {
            if (*(short *)(base + j * 0x20 + 0x1a) != 0)
                func_ov004_020adf2c(p);
            p += 0x20;
        }
    }

    if (*(int *)(self + 0xa4) == 0) {
        if (((Scene *)data_0209f5bc)->f05() == 0)
            return 0;
    }

    _Z14ApproachLinearRiii((int *)(self + 0xac), *(int *)(self + 0xa8), data_0208ee44);

    {
        int *p = (int *)(((int)self + 0x5c) & 0xFFFFFFFFFFFFFFFF);
        *p = *p + 1;
    }
    if (*(int *)(self + 0x5c) >= 0x28)
        *(int *)(self + 0x5c) = 0;

    return 1;
}
