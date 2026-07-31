#include "types.h"
/* SAVE_DATA layout (partial):
 *   0x04 = flags    (u32)
 *   0x41 = charID   (u8)
 */
typedef struct {
    char pad0[0x4];
    u32 flags;       /* 0x04 */
    char pad1[0x39]; /* 0x08..0x40 */
    u8 charID;       /* 0x41 */
} SaveData;

extern SaveData SAVE_DATA; /* 0x0209caa0 */
extern int _ZN8SaveData16CanPlayerHaveCapEv(void); /* 0x02013b5c */

void func_020139b8(void)
{
    if (!_ZN8SaveData16CanPlayerHaveCapEv())
        return;
    u8 id = SAVE_DATA.charID;
    SAVE_DATA.flags &= ~(0x8000000u << id);
}
