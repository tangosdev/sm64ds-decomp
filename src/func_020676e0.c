extern void Crash(void);
#define LD(x) (((int)(x)) & 0xFFFFFFFFFFFFFFFFLL)
void func_020676e0(unsigned *self, unsigned *mode_p, unsigned *out, unsigned *cursor)
{
    unsigned mode = *mode_p;
    unsigned *p;
    unsigned addr, size, end;
    unsigned bad, special;
    unsigned *f;

    if (mode == 0) goto case0;
    if (mode == 1) goto case1;
    if (mode == 2) goto case2;
    return;

case0:
    p = (unsigned *)LD((int)self + 0x28);
    addr = *p;
    if (addr < 0x2000000u) goto crash0;
    if (addr >= 0x22c0000u) goto crash0;
    size = p[1];
    if (addr + size > 0x22c0000u) goto crash0;
    out[2] = size;
    out[1] = *p;
    f = (unsigned *)LD((int)out + 0xc);
    out[0] = out[1];
    *f = *f & ~1u;
    return;
crash0:
    Crash();
    return;

case1:
    p = (unsigned *)LD((int)self + 0x38);
    addr = *p;
    size = p[1];
    bad = 0;
    special = bad;
    {
        unsigned e = addr + size;
        end = e;
    }
    if (addr < 0x2000000u) goto region2;
    if (addr >= 0x23fe800u) goto region2;
    if (end <= 0x2300000u) goto check_bad;
    if (end >= 0x23fe800u) goto set_bad;
    if (size <= 0x40000u) { special = 1; goto check_bad; }
set_bad:
    bad = 1;
    goto check_bad;
region2:
    if (addr < 0x37f8000u) goto set_bad2;
    if (addr >= 0x380f000u) goto set_bad2;
    if (end <= 0x380f000u) { special = 1; goto check_bad; }
    bad = 1;
    goto check_bad;
set_bad2:
    bad = 1;
check_bad:
    if (bad == 1) Crash();
    out[2] = p[1];
    out[1] = p[0];
    if (special == 0) out[0] = out[1];
    else { out[0] = *cursor; *cursor = *cursor + out[2]; }
    f = (unsigned *)LD((int)out + 0xc);
    *f = (*f & ~1u) | 1u;
    return;

case2:
    out[2] = 0x160;
    out[1] = 0x027ffe00u;
    out[0] = out[1];
    f = (unsigned *)LD((int)out + 0xc);
    *f = *f & ~1u;
    return;
}
