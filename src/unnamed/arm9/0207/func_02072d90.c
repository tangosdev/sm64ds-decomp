struct S72d90 {
    int pad[2];
    unsigned char *p;
};

unsigned char func_02072d90(struct S72d90 *c)
{
    unsigned char *p = c->p;
    int r;
    if (p)
        r = p[0] & 0x1f;
    else
        r = 0;
    return (unsigned char)r;
}
