extern int DecIfAbove0_Short(void *);
extern int _ZN5Actor13DistToCPlayerEv(void *);
extern int func_ov102_02149da8(void *, int);
int func_ov102_02149878(char *c)
{
    int r = DecIfAbove0_Short(c + 0x3ee);
    if (r != 0) return r;
    r = _ZN5Actor13DistToCPlayerEv(c);
    if (r <= 0x64000) return r;
    return func_ov102_02149da8(c, 0);
}
