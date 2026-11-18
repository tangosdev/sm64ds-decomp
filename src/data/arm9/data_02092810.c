/* dScene RTTI, gNextSceneId, the dWipe record and the message tables.
 * gNextSceneId selects the scene dScene_c::SpawnIfNecessary starts.
 * Consumer: src/game/stages/d_scene.cpp, d_wipe.cpp, Message3.cpp.
 * ROM 0x02092810..0x02092824, 20 bytes, 2 configured symbols.
 */
#include "types.h"

/* 0x02092810: 8 bytes. */
u8 data_02092810[8] = {
    0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfc, 0xfe, 0x00,
};

/* 0x02092818: 12 bytes. */
u8 data_02092818[12] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
};
