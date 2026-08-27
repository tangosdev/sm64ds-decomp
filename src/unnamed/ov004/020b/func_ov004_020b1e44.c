extern void func_ov004_020b2444(int a, int b, int c, int d, int e, int f, int g);
extern int GetGameLanguage(void);
extern void DrawOamSprite(int a, int b, int c, int d);
extern int* data_ov004_020bbfa8[];
void func_ov004_020b1e44(int arg) {
    int r;
    func_ov004_020b2444(0x80, 0xc, arg, 1, -1, 0, 0);
    r = GetGameLanguage();
    DrawOamSprite(data_ov004_020bbfa8[r][3], 0x60, 0xc, 0);
}
