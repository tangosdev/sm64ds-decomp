#include "types.h"
/* func_02013a88 at 0x02013a88
 * SaveData: clear 0x1000000 << currentCharacter bit in flags1, then SaveCurrentFile.
 * Player gets cap back (persisted).
 */
struct SaveData {
    u32 magic8000;
    u32 flags1;
    char pad[0x39];
    u8 currentCharacter;
};

extern int _ZN8SaveData16CanPlayerHaveCapEv(void);
extern int func_020139b8(void);
extern struct SaveData SAVE_DATA;

void func_02013a88(void)
{
    if (!_ZN8SaveData16CanPlayerHaveCapEv())
        return;
    SAVE_DATA.flags1 = SAVE_DATA.flags1 & ~(0x1000000u << SAVE_DATA.currentCharacter);
    func_020139b8();
}
