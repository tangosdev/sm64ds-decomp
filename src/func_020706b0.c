typedef unsigned char u8;
typedef unsigned int u32;

char *func_020706b0(char *dst, const char *src) {
    u8 *s = (u8*)src;
    u8 *d = (u8*)dst;
    u8 *d2 = (u8*)dst;
    u32 a;
    if (((u32)dst & 3) == ((u32)src & 3)) {
        a = ((u32)src & 3) & 0xFFFFFFFFFFFFFFFFull;
        if (a != 0) {
            *d = *s;
            if (*d == 0) return dst;
            a = 3 - a;
            if (a != 0) {
                do {
                    *++d2 = *++s;
                    if (*d2 == 0) return dst;
                } while (--a);
            }
            d2++;
            s++;
        }
        {
            u32 w = *(u32*)s;
            if (((w + 0xfefefeffu) & 0x80808080u) == 0) {
                d2 -= 4;
                do {
                    d2 += 4;
                    *(u32*)d2 = w;
                    s += 4;
                    w = *(u32*)s;
                } while (((w + 0xfefefeffu) & 0x80808080u) == 0);
                d2 += 4;
            }
        }
    }
    *d2 = *s;
    if (*d2 == 0) return dst;
    do {
        *++d2 = *++s;
    } while (*d2 != 0);
    return dst;
}
