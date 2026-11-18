/* dScene RTTI, gNextSceneId, the dWipe record and the message tables.
 * gNextSceneId selects the scene dScene_c::SpawnIfNecessary starts.
 * Consumer: src/game/stages/d_scene.cpp, d_wipe.cpp, Message3.cpp.
 * ROM 0x020927ec..0x02092808, 28 bytes, 1 configured symbols.
 */
#include "types.h"

extern void func_02030dac();
extern void func_02030ca0();
extern void func_02030c38();
extern void func_02030b58();

/* 0x020927ec: 28 bytes. */
void (*data_020927ec[])(void) = {
    0,
    (void (*)(void))&func_02030dac,
    0,
    (void (*)(void))&func_02030ca0,
    (void (*)(void))&func_02030c38,
    (void (*)(void))&func_02030b58,
    0,
};
