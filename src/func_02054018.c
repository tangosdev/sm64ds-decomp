#include "types.h"
extern u16 DisableVramBanks(u16*);
extern u16 data_020a608a;

u16 func_02054018(void) {
    return DisableVramBanks(&data_020a608a);
}
