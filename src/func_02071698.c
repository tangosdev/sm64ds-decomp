typedef unsigned char u8;

int func_02071698(u8* base, int idx)
{
    u8* p = base + 5 + idx;
    u8 e = *p;
    u8* end;
    if (e > 5) return 1;
    if (e < 5) return -1;
    p = p + 1;
    end = base + 5 + base[4];
    while (p < end) {
        if (*p != 0) return 1;
        p = p + 1;
    }
    return ((base + (idx - 1))[5] & 1) ? 1 : -1;
}
