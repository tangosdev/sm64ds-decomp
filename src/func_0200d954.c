extern int func_02053200(int x);
extern int Clipper_Func_020156DC(void *tab, int a, int b, int d, int e);
extern short data_02082214[];
extern int data_0209f43c;

void func_0200d954(char *c, short arg) {
    int i = (int)(unsigned short)arg >> 4;
    int j = i * 2;
    *(int *)(c + 0x104) = func_02053200(data_02082214[j]);
    *(int *)(c + 0x108) = func_02053200(data_02082214[j + 1]);
    Clipper_Func_020156DC(&data_0209f43c, *(int *)(c + 0xf8), arg, *(int *)(c + 0xfc), *(int *)(c + 0x100));
}
