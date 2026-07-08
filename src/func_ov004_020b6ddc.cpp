//cpp
// NONMATCHING: different op / idiom (div=46). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;

extern "C" void func_ov004_020b0aa0(int arg);
extern "C" void func_02012790(int x);
extern "C" void func_ov004_020b7460(void *c);
extern "C" int func_ov004_020b04c0(void);
extern "C" void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
extern "C" void func_ov004_020ae274(void *c);

struct Vtbl { void (*fns[64])(); };
struct Obj { Vtbl *vt; };

extern "C" u8 data_020a0e40[];
extern "C" u8 TOUCH_INPUT_ARR[];
extern "C" u8 data_020a0de9[];
extern "C" int data_02086b58[];
extern "C" Obj *data_ov004_020beb68;
extern "C" int data_ov004_020bc8a0;
extern "C" int data_ov004_020bc864;
extern "C" int data_ov004_020bc888;

extern "C" void func_ov004_020b6ddc(int *c) {
    u8 i;
    int flag;
    Obj *o;
    int v;
    int s;
    int r4;
    if (c[0x1c / 4] == 0) {
        i = data_020a0e40[0];
        flag = 0;
        if (TOUCH_INPUT_ARR[i * 4] != 0 && data_020a0de9[i * 4] != 0)
            flag = 1;
        if (flag != 0) {
            func_ov004_020b0aa0(7);
            func_02012790(0x62);
            c[0x10 / 4] = data_02086b58[0];
            c[0x14 / 4] = data_02086b58[1];
            func_ov004_020b7460(c);
            if (c[0x1c / 4] != 0xb4)
                return;
            o = data_ov004_020beb68;
            if (o)
                (o->vt->fns[0x14])();
            v = data_ov004_020bc8a0;
            if (v != 0x1d)
                data_ov004_020bc8a0 = 0x1d;
            r4 = (v != 0x1d) ? v : 5;
            s = data_ov004_020bc864;
            if (s < 0)
                s -= func_ov004_020b04c0();
            func_ov004_020b0cac(r4, data_ov004_020bc888, s, -1, -1, 0xd);
            func_ov004_020ae274((void *)1);
        }
    }
}
