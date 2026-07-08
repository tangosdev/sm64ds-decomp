extern int func_02053200(int x);
extern int Clipper_Func_020156DC(void *tab, int a, int b, int d, int e);
extern short SINE_TABLE[];
extern int GLOBAL_CLIPPER;

void func_0200d954(char *c, short arg) {
    int i = (int)(unsigned short)arg >> 4;
    int j = i * 2;
    *(int *)(c + 0x104) = func_02053200(SINE_TABLE[j]);
    *(int *)(c + 0x108) = func_02053200(SINE_TABLE[j + 1]);
    Clipper_Func_020156DC(&GLOBAL_CLIPPER, *(int *)(c + 0xf8), arg, *(int *)(c + 0xfc), *(int *)(c + 0x100));
}
