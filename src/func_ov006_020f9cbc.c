struct Obj
{
    char pad[0x2a];
    short f2a;
    unsigned char f2c;
};

int func_ov006_020f9cbc(struct Obj *a, struct Obj *b)
{
    int ai, bi, dm, dd;
    if (b->f2c != a->f2c)
        goto fail;
    bi = b->f2a;
    ai = a->f2a;
    dm = ai % 5 - bi % 5;
    dd = ai / 5 - bi / 5;
    if (dm < 0)
        dm = -dm;
    if (dm >= 2)
        goto fail;
    if (dd < 0)
        dd = -dd;
    if (dd >= 2)
        goto fail;
    return 1;
fail:
    return 0;
}
