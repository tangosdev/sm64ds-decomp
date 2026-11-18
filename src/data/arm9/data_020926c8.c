/* dScene RTTI, gNextSceneId, the dWipe record and the message tables.
 * gNextSceneId selects the scene dScene_c::SpawnIfNecessary starts.
 * Consumer: src/game/stages/d_scene.cpp, d_wipe.cpp, Message3.cpp.
 * ROM 0x020926c8..0x020926d0, 8 bytes, 2 configured symbols.
 */
#include "types.h"

/* 0x020926c8: 4 bytes. */
u8 data_020926c8[4] = {
    0xff,
    0x7f,
    0x00,
    0x00,
};

/* 0x020926cc: 4 bytes. */
u8 data_020926cc[4] = {
    0xff,
    0x7f,
    0x00,
    0x00,
};
