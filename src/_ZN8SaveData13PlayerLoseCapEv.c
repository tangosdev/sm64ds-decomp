#include "types.h"
/* _ZN8SaveData13PlayerLoseCapEv at 0x02013ad4
 * SaveData::PlayerLoseCap - sets 0x1000000 << currentCharacter bit in flags1.
 */
struct SaveData {
    u32 magic8000;
    u32 flags1;
    char pad[0x39];
    u8 currentCharacter;
};

extern int _ZN8SaveData16CanPlayerHaveCapEv(void);
extern struct SaveData data_0209caa0;

void _ZN8SaveData13PlayerLoseCapEv(void)
{
    if (!_ZN8SaveData16CanPlayerHaveCapEv())
        return;
    data_0209caa0.flags1 = data_0209caa0.flags1 | (0x1000000u << data_0209caa0.currentCharacter);
}
