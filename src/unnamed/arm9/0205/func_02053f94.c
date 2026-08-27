#include "types.h"
extern u16 DisableVramBanks(u16*);
extern u16 data_020a6092;

u16 func_02053f94(void) {
    return DisableVramBanks(&data_020a6092);
}
