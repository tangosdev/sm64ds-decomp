extern int func_02071644(void *out, int n);

struct S {
    unsigned char b0;       /* [0] */
    unsigned char pad1;     /* [1] */
    short s2;               /* [2] */
    unsigned char b4;       /* [4] */
    unsigned char data[1];  /* [5] */
};

void func_020712a0(struct S *out, unsigned char *str, short val)
{
    int i;
    unsigned char c;
    out->s2 = val;
    out->b0 = 0;
    i = 0;
    while (i < 0x20 && str[0] != 0) {
        out->data[i++] = (unsigned char)(*str++ - 0x30);
    }
    out->b4 = (unsigned char)i;
    if (str[0] == 0) return;
    if (str[0] < 5) return;
    if (str[0] > 5) goto docall;
    c = str[1];
    str++;
    while (c != 0) {
        if (c != 0x30) goto docall;
        c = *++str;
    }
    if ((out->data[i - 1] & 1) == 0) return;
docall:
    func_02071644(out, out->b4);
}
