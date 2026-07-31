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
extern struct SaveData SAVE_DATA;

void _ZN8SaveData13PlayerLoseCapEv(void)
{
    if (!_ZN8SaveData16CanPlayerHaveCapEv())
        return;
    SAVE_DATA.flags1 = SAVE_DATA.flags1 | (0x1000000u << SAVE_DATA.currentCharacter);
}
