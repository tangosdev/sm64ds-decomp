extern void FreeGfxSlotsById(int arg);
extern void func_02012790(int x);
extern void func_ov004_020b7460(void *c);
extern int func_ov004_020b04c0(void);
extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
extern void func_ov004_020ae274(int x);

struct P2 { int a, b; };

extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[][4];
extern unsigned char data_020a0de9[][4];
extern struct P2 data_02086b58;
extern void *data_ov004_020beb68;
extern int data_ov004_020bc8a0;
extern int data_ov004_020bc864;
extern int data_ov004_020bc888;

void func_ov004_020b6ddc(char *c)
{
    int idx;
    int flag;
    void *o;
    int v;
    int s;
    int r4;

    if (*(int *)(c + 0x1c) == 0) {
        idx = data_020a0e40;
        flag = 0;
        if (data_020a0de8[idx][0] != 0) {
            if (data_020a0de9[idx][0] != 0)
                flag = 1;
        }
        if (flag != 0) {
            FreeGfxSlotsById(7);
            func_02012790(0x62);
            *(struct P2 *)(c + 0x10) = data_02086b58;
            func_ov004_020b7460(c);
        }
    }

    if (*(int *)(c + 0x1c) != 0xb4)
        return;

    o = data_ov004_020beb68;
    if (o) {
        void (*m50)(void *) = *(void (**)(void *))(*(unsigned int *)o + 0x50);
        m50(o);
    }

    r4 = 5;
    v = data_ov004_020bc8a0;
    if (v != 0x1d) {
        data_ov004_020bc8a0 = 0x1d;
        r4 = v;
    }

    s = data_ov004_020bc864;
    if (s < 0)
        s -= func_ov004_020b04c0();

    func_ov004_020b0cac(r4, data_ov004_020bc888, s, -1, -1, 0xd);
    func_ov004_020ae274(1);
}
