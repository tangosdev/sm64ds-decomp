//cpp
// @symbol _ZN7daOts_c16OnAimedAtWithEggEv

#include "daOts_c.h"

struct BullyResourceConfig {
    u8 pad_00[0x24];
    Fix12i eggAimSpeed;
};

int daOts_c::OnAimedAtWithEgg()
{
    BullyResourceConfig *config = (BullyResourceConfig *)mFileTable;
    Fix12i aimSpeed = 0x14000;
    if (config != 0)
        aimSpeed = config->eggAimSpeed;

    return aimSpeed;
}
