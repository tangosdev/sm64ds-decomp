extern int fn_02389898(int a1);
extern volatile unsigned short *fn_02389958(int a1, int a2);
#pragma opt_propagation off

int fn_02389848(int a1) {
    int save = a1;
    int idx = fn_02389898(0);
    idx &= ~save;
    int b = idx & 0xFF;
    int z = 0;
    return (int)fn_02389958(z, b);
}
