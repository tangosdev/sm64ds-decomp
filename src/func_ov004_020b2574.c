extern int func_ov004_020ad674(void);
extern void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3);
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void func_ov004_020af948(void *a, int b, int c, void *m);
extern void **data_ov004_020bbfa8[];

void func_ov004_020b2574(int arg0, int arg1)
{
    void **table = data_ov004_020bbfa8[func_ov004_020ad674()];
    void *sb = table[0];
    int b = 12;
    if (arg1 != 1) {
        DrawOamSprite(sb, (void *)b, b, 0);
        DrawOamSprite(data_ov004_020bbfa8[func_ov004_020ad674()][1], (void *)30, b, 0);
        func_ov004_020b2444(0x30, b, arg0, 1, -1, 2, 0);
    } else {
        int i;
        for (i = 0; i < arg0; i++) {
            func_ov004_020af948(sb, b, 12, 0);
            b += 16;
        }
    }
}
