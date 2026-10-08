extern int __aeabi_idiv(int a, int b);

struct T {
    short v0;       /* [0] */
    short v2;       /* [2] */
    short v4;       /* [4] */
    unsigned char b6;  /* [6] */
    unsigned char b7;  /* [7] */
};

void func_0204d440(short *out, int **pp, int t)
{
    struct T *p = (struct T *)pp[1];
    unsigned char n1 = p->b6;
    unsigned char n2 = p->b7;
    if (t < n1) {
        short a = p->v0;
        out[0x1a] = a + __aeabi_idiv(t * (p->v2 - a), n1);
        return;
    }
    if (t < n2) {
        out[0x1a] = p->v2;
        return;
    }
    {
        short a = p->v4;
        out[0x1a] = a + __aeabi_idiv((t - 0xff) * (a - p->v2), 0xff - n2);
    }
}
