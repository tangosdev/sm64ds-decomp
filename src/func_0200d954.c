extern int func_02053200(int x);
extern void _ZN7Clipper13Func_020156DCEitii(void *tab, int a, int b, int d, int e);
extern short data_02082214[];
extern int data_0209f43c;

void func_0200d954(char *c, short arg) {
    int i = (int)(unsigned short)arg >> 4;
    int j = i * 2;
    *(int *)(c + 0x104) = func_02053200(data_02082214[j]);
    *(int *)(c + 0x108) = func_02053200(data_02082214[j + 1]);
    _ZN7Clipper13Func_020156DCEitii(&data_0209f43c, *(int *)(c + 0xf8), arg, *(int *)(c + 0xfc), *(int *)(c + 0x100));
}
