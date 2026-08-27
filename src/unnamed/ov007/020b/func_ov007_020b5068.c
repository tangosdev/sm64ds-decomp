extern int func_ov007_020c1da0(int i);
extern int func_ov007_020b45b0(void *t);
extern char *data_ov007_0210342c;

int func_ov007_020b5068(void *t)
{
    unsigned short id = **(unsigned short **)t;
    unsigned short *p;
    int r;

    if (id != 8 && id != 0x14 && id != 0x17)
        return id;

    p = *(unsigned short **)(data_ov007_0210342c + 0x54);
    if (!(p[0] & 2))
        return p[0] & 2;
    if (p[1] & 2)
        return p[1] & 2;
    r = func_ov007_020c1da0(0);
    if (r)
        return r;
    return func_ov007_020b45b0(t);
}
