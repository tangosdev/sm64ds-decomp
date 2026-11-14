/* storage 0x02391D0C runtime 0x027C3C84 size 0x84 main ARM. Message 0xB builder
 * (sibling of 91D90/91F54): object via +0x1610 double-deref, half extract
 * LSL#16/LSR#16, message halves 0x11/0/0xE. Needs #pragma opt_propagation off
 * (AR25-0604 lever) + named obj to keep the object in ip. */

#pragma opt_propagation off

extern int *fn_0238E688(void);
extern int fn_0238DE4C(int a1);
extern int fn_0238DEF4(int a1);

struct Obj {
    short h0;
    char _2[0x12];
    int w14;
    char _18[8];
    int w20;
    char _24[0x80];
    int wA4;
    short hA8;
    short hAA;
    int wAC;
    int wB0;
    unsigned short hB4;
};

struct Arg {
    int _0;
    int w4;
    int w8;
};

int fn_02391D0C(struct Arg *a1) {
    struct Obj *obj;
    int v3;
    int w;
    int half;
    int msg;

    obj = *(struct Obj **)*(int *)((char *)fn_0238E688() + 0x1610);
    v3 = a1->w4;
    w = a1->w8;
    half = (unsigned short)w;
    obj->w14 = 1;
    obj->wAC = v3;
    obj->hB4 = (unsigned short)half;
    obj->wB0 = v3 + half;
    obj->hAA = 0;
    obj->wA4 = 0;
    obj->hA8 = 0;
    obj->w20 = 0;
    obj->h0 = 11;
    msg = fn_0238DE4C(11);
    *(short *)(msg + 0) = 0x11;
    *(short *)(msg + 2) = 0;
    *(short *)(msg + 4) = 14;
    return fn_0238DEF4(msg);
}
