#include "types.h"
// @symbol func_ov002_020afa98
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 id, struct Vector3 *v);
extern int func_ov002_020af248(char *c, int n);

void func_ov002_020afa98(char *c)
{
    switch (*(s32 *)(c + 0x388)) {
    case 0:
        *(u8 *)(c + 0x38e) = 0;
        if (*(s32 *)(c + 0x390) != 0)
            return;
        *(u32 *)(c + 0xa8) = 0x28000;
        *(s32 *)(c + 0x388) = 3;
        *(u8 *)(c + 0x38e) = 1;
        func_ov002_020aefa4(c);
        _ZN5Sound9PlayBank3EjRK7Vector3(0x68, (struct Vector3 *)(c + 0x74));
        *(u32 *)(c + 0xb0) &= ~1;
        return;
    case 1:
        func_ov002_020aefb8(c);
        func_ov002_020afde4(c);
        func_ov002_020aeee4(c);
        return;
    case 2:
        func_ov002_020aefb8(c);
        func_ov002_020af3a8(c);
        func_ov002_020af248(c, 0x1e);
        return;
    case 3:
        func_ov002_020aefb8(c);
        if (*(u16 *)(c + 0x100) > 0x11) {
            func_ov002_020aeee4(c);
        }
        func_ov002_020af474(c);
        if (*(u16 *)(c + 0x100) != 0x25)
            return;
        *(u32 *)(c + 0x128) &= ~1;
        *(s32 *)(c + 0x388) = 1;
        *(u32 *)(c + 0x98) = 0x8000;
        return;
    }
}
