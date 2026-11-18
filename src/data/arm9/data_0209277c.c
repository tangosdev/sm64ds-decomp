/* dScene RTTI, gNextSceneId, the dWipe record and the message tables.
 * gNextSceneId selects the scene dScene_c::SpawnIfNecessary starts.
 * Consumer: src/game/stages/d_scene.cpp, d_wipe.cpp, Message3.cpp.
 * ROM 0x0209277c..0x020927ec, 112 bytes, 9 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern u8 data_02092780[8];
extern u8 data_02092788[8];
extern u8 data_02092790[8];
extern u8 data_02092798[8];
extern u8 data_020945f0[];
extern u8 data_02094be4[];
extern u8 data_020951c4[];
extern u8 data_020957f8[];
extern u8 data_02095dfc[];
extern u8 data_02096394[];
extern u8 data_020965e0[];
extern u8 data_02096808[];
extern u8 data_02096a7c[];
extern u8 data_02096cb4[];
extern u8 data_02097fd4[];
extern u8 data_02098214[];
extern u8 data_0209843c[];
extern u8 data_020986e0[];
extern u8 data_02098918[];

/* 0x020927b0: 20 bytes. */
void *data_020927b0[5] = {
    (void *)&data_020945f0, (void *)&data_02094be4, (void *)&data_020951c4,
    (void *)&data_020957f8, (void *)&data_02095dfc,
};

/* 0x020927c4: 20 bytes. */
void *data_020927c4[5] = {
    (void *)&data_02096394, (void *)&data_020965e0, (void *)&data_02096808,
    (void *)&data_02096a7c, (void *)&data_02096cb4,
};

/* 0x020927a0: 16 bytes. */
u32 data_020927a0[4] = {
    (u32)data_02092788,
    (u32)data_02092790,
    (u32)data_02092798,
    (u32)data_02092780,
};

/* 0x02092798: 8 bytes. */
u8 data_02092798[8] = {
    0xf8, 0x00, 0xf8, 0x41, 0x98, 0x50, 0xff, 0xff,
};

/* 0x02092790: 8 bytes. */
u8 data_02092790[8] = {
    0xf8, 0x00, 0xf8, 0x41, 0x96, 0x50, 0xff, 0xff,
};

/* 0x02092788: 8 bytes. */
u8 data_02092788[8] = {
    0xf8, 0x00, 0xf8, 0x41, 0x94, 0x50, 0xff, 0xff,
};

/* 0x02092780: 8 bytes. */
u8 data_02092780[8] = {
    0xf8, 0x00, 0xf8, 0x41, 0x9c, 0x50, 0xff, 0xff,
};

/* 0x0209277c: 4 bytes. */
int data_0209277c = 0x00000001;

/* 0x020927d8: 20 bytes. */
void *data_020927d8[5] = {
    (void *)&data_02097fd4, (void *)&data_02098214, (void *)&data_0209843c,
    (void *)&data_020986e0, (void *)&data_02098918,
};
