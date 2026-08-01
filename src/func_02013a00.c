#include "types.h"
/* data_0209caa0 layout (partial):
 *   0x04 = flags    (u32)
 *   0x41 = charID   (u8)
 */
typedef struct {
    char pad0[0x4];
    u32 flags;       /* 0x04 */
    char pad1[0x39]; /* 0x08..0x40 */
    u8 charID;       /* 0x41 */
} SaveData;

extern SaveData data_0209caa0; /* 0x0209caa0 */
extern int _ZN8SaveData16CanPlayerHaveCapEv(void); /* 0x02013b5c */

void func_02013a00(void)
{
    if (!_ZN8SaveData16CanPlayerHaveCapEv())
        return;
    u8 id = data_0209caa0.charID;
    data_0209caa0.flags |= (0x8000000u << id);
}
