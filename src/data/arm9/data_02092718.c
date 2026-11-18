/* dScene RTTI, gNextSceneId, the dWipe record and the message tables.
 * gNextSceneId selects the scene dScene_c::SpawnIfNecessary starts.
 * Consumer: src/game/stages/d_scene.cpp, d_wipe.cpp, Message3.cpp.
 * ROM 0x02092718..0x02092720, 8 bytes, 1 configured symbols.
 */
#include "types.h"

extern u32 _ZTI7dView_c[];

/* 0x02092718: 8 bytes. */
u32 data_02092718[2] = {
    0x00000000,
    (u32)_ZTI7dView_c,
};
