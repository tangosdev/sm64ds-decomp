struct S57f38 {
    char pad84[0x84];
    unsigned char *base;
    int pad88[1];
    int idx;
};

void func_02057f38(struct S57f38 *c, int v)
{
    c->idx = v;
    *(int *)(c->base + c->idx) = 0x597dfbd9;
}
