extern void *_ZN6Memory13operator_new2Ej(unsigned int sz);

int *func_ov002_020e6bb0(char *self)
{
    char **pp;
    char *base;
    unsigned int count;
    int *out;
    int *p;
    char *src;
    unsigned int i;

    pp = (char **)(((long long)(int)(self + 8)));
    base = *pp;
    count = *(unsigned int *)(base + 0x24);
    out = (int *)_ZN6Memory13operator_new2Ej(count << 2);

    p = out;
    src = pp[1];
    for (i = 0; i < count; i++) {
        *p++ = *(int *)(src + 0x1c);
        src += 0x30;
    }
    return out;
}
