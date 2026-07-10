// NONMATCHING: stack frame size (#4 vs #0xc) and velocity zeroing store shape (div~size gap)
typedef unsigned short u16;
typedef unsigned int u32;

extern void *_ZN5Actor10FindWithIDEj(u32 id);
extern int _ZN5Actor24BumpedUnderneathByPlayerER6Player(void *thiz, void *player);
extern int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(void *thiz, void *clsn, void *player);

int func_ov020_021115ac(char *c)
{
    u32 id;
    char *found;
    int t;

    id = *(u32 *)(c + 0x240);
    if (id == 0)
        return 0;
    found = (char *)_ZN5Actor10FindWithIDEj(id);
    if (found == 0)
        return 0;

    t = (int)(*(u16 *)(found + 0xc) == 0xbf);
    if (t == 0)
        return 0;

    if ((*(u32 *)(c + 0x23c) & 0x8000) != 0)
        return 0;

    if (*(unsigned char *)(found + 0x6fb) != 0) {
        t = (int)(*(u16 *)(c + 0xc) == 0x145);
        if (t != 0)
            return 0;
    }

    *(void **)(c + 0x41c) = found;

    if ((*(u32 *)(c + 0x23c) & 0x26fe0) != 0)
        return 1;

    if (_ZN5Actor24BumpedUnderneathByPlayerER6Player(c, found) != 0) {
        int *p = (int *)(found + 0xa4);
        int r2 = p[2];
        int r1 = p[0];
        int r0 = 0;
        p[0] = r1;
        p[1] = r0;
        p[2] = r2;
        *(void **)(c + 0x41c) = found;
        return 1;
    }

    if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(c, c + 0x21c, found) != 0) {
        *(void **)(c + 0x41c) = found;
        return ~1;
    }

    if (*(unsigned char *)(found + 0x6f9) != 0)
        return 2;

    if (*(unsigned char *)(found + 0x6fb) != 0) {
        t = (int)(*(u16 *)(c + 0xc) == 0x147);
        if (t != 0)
            return 0;
    }
    return ~0;
}
