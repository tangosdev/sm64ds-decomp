#include "types.h"
/* func_02013a44 at 0x02013a44
 * SaveData: test if 0x8000000 << currentCharacter bit is set in flags1.
 */
struct SaveData {
    u32 magic8000;
    u32 flags1;
    char pad[0x39];
    u8 currentCharacter;
};

extern int _ZN8SaveData16CanPlayerHaveCapEv(void);
extern struct SaveData data_0209caa0;

int func_02013a44(void)
{
    if (!_ZN8SaveData16CanPlayerHaveCapEv())
        return 0;
    return data_0209caa0.flags1 & (0x8000000u << data_0209caa0.currentCharacter);
}
