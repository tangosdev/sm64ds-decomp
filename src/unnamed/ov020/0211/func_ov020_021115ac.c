#include "types.h"
/* The ROM reserves a 12-byte frame it never touches; two address-taken volatile locals
 * reserve it with no emitted code (plain or non-address-taken volatiles are DCE'd before
 * frame layout). The 6g u64-mask launder on &vel forces the ROM's `add r0,r4,#0xa4` base. */
typedef struct { int x, y, z; } Vec3;
#define LDR(p) ((int)(p))

extern void *_ZN8dActor_c10FindWithIDEj(u32 id);
extern int _ZN8dActor_c24BumpedUnderneathByPlayerER6Player(void *thiz, void *player);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void *thiz, void *clsn, void *player);

int func_ov020_021115ac(char *c)
{
    u32 id;
    char *found;
    int t;
    volatile int t1, t2;
    (void)&t1; (void)&t2;

    id = *(u32 *)(c + 0x240);
    if (id == 0)
        return 0;
    found = (char *)_ZN8dActor_c10FindWithIDEj(id);
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

    if (_ZN8dActor_c24BumpedUnderneathByPlayerER6Player(c, found) != 0) {
        Vec3 *p = (Vec3 *)LDR(found + 0xa4);
        Vec3 v;
        v.z = p->z;
        v.x = p->x;
        v.y = 0;
        *(int *)(found + 0xa4) = v.x;
        *(int *)(found + 0xa8) = v.y;
        *(int *)(found + 0xac) = v.z;
        *(void **)(c + 0x41c) = found;
        return 1;
    }

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x21c, found) != 0) {
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
