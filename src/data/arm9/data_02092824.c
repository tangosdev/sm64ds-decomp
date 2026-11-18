/* dScene RTTI, gNextSceneId, the dWipe record and the message tables.
 * gNextSceneId selects the scene dScene_c::SpawnIfNecessary starts.
 * Consumer: src/game/stages/d_scene.cpp, d_wipe.cpp, Message3.cpp.
 * ROM 0x02092824..0x0209289c, 120 bytes, 5 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern void _Z13func_0203128cv();
extern void _ZN7Message11AddChoiceVSEv();
extern void _ZN7Message13AddCharPairVSEv();

/* 0x0209285c: 32 bytes. */
int data_0209285c[8] = {
    (int)0xffffffffu, (int)0xddddddddu, (int)0xbbbbbbbbu, (int)0x99999999u,
    0x77777777,       0x55555555,       0x33333333,       0x11111111,
};

/* 0x0209283c: 32 bytes. */
unsigned int data_0209283c[8] = {
    0x00000000u, 0x0000000fu, 0x000000ffu, 0x00000fffu,
    0x0000ffffu, 0x000fffffu, 0x00ffffffu, 0x0fffffffu,
};

/* 0x02092830: 12 bytes. */
void (*data_02092830[])(void) = {
    (void (*)(void))&_Z13func_0203128cv,
    (void (*)(void))&_ZN7Message11AddChoiceVSEv,
    (void (*)(void))&_ZN7Message13AddCharPairVSEv,
};

/* 0x02092824: 12 bytes. */
u8 data_02092824[12] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x00,
};

/* 0x0209287c: 32 bytes. */
unsigned int data_0209287c[8] = {
    0xffffffffu, 0xfffffff0u, 0xffffff00u, 0xfffff000u,
    0xffff0000u, 0xfff00000u, 0xff000000u, 0xf0000000u,
};
