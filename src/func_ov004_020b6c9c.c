struct P2 { int a; int b; };
extern void *data_ov004_020beb68;
extern unsigned char data_020a0e40[];
extern unsigned char TOUCH_INPUT_ARR[];
extern unsigned char data_020a0de9[];
extern struct P2 data_02086b58;

extern void func_ov004_020b0aa0(int arg);
extern int func_ov004_020adbe0(void);
extern void func_ov004_020b6f14(void *c);
extern void func_ov004_020b7020(char *c);

struct Obj { char pad[0x10]; struct P2 p10; int f18; int f1c; };

void func_ov004_020b6c9c(struct Obj *c)
{
    int x = c->f1c;
    void *g = data_ov004_020beb68;
    if (x != 0) {
        unsigned char idx = data_020a0e40[0];
        int off = idx * 4;
        int ok = 0;
        if (TOUCH_INPUT_ARR[off]) {
            if (data_020a0de9[off]) ok = 1;
        }
        if (ok == 0)
            return;
    }
    {
        int (*fn)(void *, int) = (int (*)(void *, int)) (*(void ***)g)[0x13];
        if (fn(g, c->f18) == 0)
            return;
    }
    c->p10 = data_02086b58;
    func_ov004_020b0aa0(0x1d);
    if (func_ov004_020adbe0() != 0) {
        func_ov004_020b6f14(c);
        return;
    }
    func_ov004_020b7020((char *)c);
}
