extern void Crash(void);
#define LD(x) (((int)(x)))
void func_020676e0(unsigned *self, unsigned *mode_p, unsigned *out, unsigned *cursor)
{
    unsigned mode = *mode_p;
    unsigned *p;
    unsigned addr, size, end;
    unsigned bad, special;
    unsigned *f;

    switch (mode) {
    case 0:
        p = (unsigned *)LD((int)self + 0x28);
        addr = *p;
        if (addr >= 0x2000000u && addr < 0x22c0000u) {
            size = p[1];
            if (addr + size <= 0x22c0000u) {
                out[2] = size;
                out[1] = *p;
                f = (unsigned *)LD((int)out + 0xc);
                out[0] = out[1];
                *f = *f & ~1u;
                return;
            }
        }
        Crash();
        return;

    case 1:
        p = (unsigned *)LD((int)self + 0x38);
        addr = *p;
        size = p[1];
        bad = 0;
        special = bad;
        {
            unsigned e = addr + size;
            end = e;
        }
        if (addr >= 0x2000000u && addr < 0x23fe800u) {
            if (end > 0x2300000u) {
                if (end < 0x23fe800u && size <= 0x40000u)
                    special = 1;
                else
                    bad = 1;
            }
        } else if (addr >= 0x37f8000u && addr < 0x380f000u) {
            if (end <= 0x380f000u)
                special = 1;
            else
                bad = 1;
        } else {
            bad = 1;
        }
        if (bad == 1)
            Crash();
        out[2] = p[1];
        out[1] = p[0];
        if (special == 0)
            out[0] = out[1];
        else {
            out[0] = *cursor;
            *cursor = *cursor + out[2];
        }
        f = (unsigned *)LD((int)out + 0xc);
        *f = (*f & ~1u) | 1u;
        return;

    case 2:
        out[2] = 0x160;
        out[1] = 0x027ffe00u;
        out[0] = out[1];
        f = (unsigned *)LD((int)out + 0xc);
        *f = *f & ~1u;
        break;
    }
}
