//cpp
/* dScene_c::PrepareToSpawnBoot() at 0x0202e300, 0x48 bytes.
 *
 * Static. Selects which scene boots next -- 0 when 0x0203d9b4 says so, otherwise
 * 0x168 -- and clears the "already spawned" flag so SpawnIfNecessary will act. */
#include "dScene_c.h"

extern "C" {
extern int func_0203d9b4(void);
extern u16 data_02092664;   /* pending scene ID */
extern u8  data_02092660;   /* set once the pending scene has been spawned */
}

void dScene_c::PrepareToSpawnBoot()
{
    if (func_0203d9b4())
        data_02092664 = 0;
    else
        data_02092664 = 0x168;
    data_02092660 = 0;
}
