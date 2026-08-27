struct S {
    char pad[0x14];
    int *bits; /* 0x14 */
};
int func_020647d8(struct S *p, int i)
{
    int w = p->bits[i >> 5];
    i = 1 << (i & 0x1f);
    return (w & i) != 0;
}
