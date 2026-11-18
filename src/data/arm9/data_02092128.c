/* Progress and limit words shared by the stage and actor code.
 * data_02092128 is the four-byte sublevel table, data_0209212c the +inf clamp
 * the player code compares against, data_02092130/data_02092134 the stage
 * entry words, and data_02092138 the Y limit the standing actors test.
 * Consumers: d_a_ply_methods, d_s_stage_methods, LevelObjects, the actors.
 * ROM 0x02092128..0x0209213c, 20 bytes, 5 configured symbols.
 *
 * Order: mwcc emits one .data section per initialized object and the linker
 * concatenates them in declaration order, so the definitions below are in the
 * order that yields the ascending addresses above.
 */
#include "types.h"

/* 0x02092134: 4 bytes. */
unsigned int data_02092134 = 0x44444444u;

/* 0x02092130: 4 bytes. */
int data_02092130 = (int)0xffffffffu;

/* 0x0209212c: 4 bytes. */
int data_0209212c = 0x7fffffff;

/* 0x02092128: 4 bytes. */
u8 data_02092128[4] = {
    0x00,
    0x01,
    0x02,
    0x03,
};

/* 0x02092138: 4 bytes. */
int data_02092138 = (int)0x80000000u;
