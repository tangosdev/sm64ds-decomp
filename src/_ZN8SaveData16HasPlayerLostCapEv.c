#include "types.h"
/* _ZN8SaveData16HasPlayerLostCapEv at 0x02013b18
 * SaveData::HasPlayerLostCap - tests 0x1000000 << currentCharacter bit in flags1.
 */
struct SaveData {
    u32 magic8000;
    u32 flags1;
    char pad[0x39];
    u8 currentCharacter;
};

extern int _ZN8SaveData16CanPlayerHaveCapEv(void);
extern struct SaveData SAVE_DATA;

int _ZN8SaveData16HasPlayerLostCapEv(void)
{
    if (!_ZN8SaveData16CanPlayerHaveCapEv())
        return 0;
    return SAVE_DATA.flags1 & (0x1000000u << SAVE_DATA.currentCharacter);
}
