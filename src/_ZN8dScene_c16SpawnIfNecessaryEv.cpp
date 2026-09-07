//cpp
/* dScene_c::SpawnIfNecessary() at 0x0202e26c, 0x94 bytes.
 *
 * Static. Spawns the pending scene actor unless one has already been spawned
 * (data_02092660) or the pending ID is the 0x187 sentinel meaning "none". On
 * success it writes the sentinel back and raises the flag, so the work happens
 * once. */
#include "dScene_c.h"

extern "C" {
extern u8  data_02092660;   /* set once the pending scene has been spawned */
extern u16 data_02092664;   /* pending scene ID; 0x187 means none */
extern u32 data_0209f5b8;   /* its parameter word */
extern int func_02013edc(u32 sceneID, u32 param, int a);
}

int dScene_c::SpawnIfNecessary()
{
    u16 sceneID;
    if (data_02092660 != 0 || (sceneID = data_02092664) == 0x187)
        return 0;
    {
        int spawned = func_02013edc(sceneID, data_0209f5b8, 1);
        if (spawned == 0)
            return 0;
        data_02092664 = 0x187;
        data_02092660 = 1;
        return spawned;
    }
}
