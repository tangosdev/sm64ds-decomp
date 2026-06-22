typedef unsigned int u32;
extern void func_020676e0(void *in, void *tbl, void *dst, void *aux);
extern void func_0205a61c(void *dst, void *src, u32 size);
extern int data_0208685c[];
void func_020678a4(int *out, int *in, void *dst)
{
    int aux;
    char *p;
    int *t;
    int i;
    aux = 0x22c0000;
    out[0] = in[9];
    out[1] = in[0xd];
    p = (char *)out + 0xc;
    t = data_0208685c;
    for (i = 0; i < 3; i++) {
        func_020676e0(in, t, p, &aux);
        p += 0x10;
        t += 1;
    }
    func_0205a61c(dst, (char *)out + 0x3c, 0x88);
}
