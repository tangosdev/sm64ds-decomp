//cpp
extern "C" {
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
int func_020124c4(int a, int b, int c, int d);
}
extern int data_020a0e68;

struct Obj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
};

struct C;
typedef void (C::*PMF)();

struct M48 { int w[12]; };

struct C {
    PMF pmf;            /* 0x0 */
    char pad8[0x10 - 8];
    Obj obj;            /* 0x10 */
};

extern "C" void func_ov006_02107db8(C *c)
{
    char *b = (char *)c;
    short *p = (short *)(((int)b + 0xc2) & 0xFFFFFFFFFFFFFFFF);
    *p = (short)(*p + *(short *)(b + 0xc8));
    *(short *)(*(int *)(b + 0x20) + 0x82) = (short)(-*(short *)(b + 0xc2));
    c->obj.v3();
    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    *(M48 *)(b + 0x2c) = *(M48 *)&data_020a0e68;
    (c->*(c->pmf))();
    {
        short s = *(short *)(b + 0xcc);
        if (s == 0)
            return;
        if (s == 2)
            *(int *)(b + 0xbc) = func_020124c4(*(int *)(b + 0xbc), 2, 0x158, 0);
        else
            *(int *)(b + 0xbc) = func_020124c4(*(int *)(b + 0xbc), 2, 0x157, 0);
    }
}
