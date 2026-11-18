/* dScene RTTI, gNextSceneId, the dWipe record and the message tables.
 * gNextSceneId selects the scene dScene_c::SpawnIfNecessary starts.
 * Consumer: src/game/stages/d_scene.cpp, d_wipe.cpp, Message3.cpp.
 * ROM 0x02092808..0x02092810, 8 bytes, 1 configured symbols.
 */
#include "types.h"

/* 0x02092808: 8 bytes. */
unsigned short data_02092808[4] = {
    0x0064,
    0x000a,
    0x0001,
    0x0000,
};
