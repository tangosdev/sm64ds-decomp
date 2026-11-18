/* Minigame group selector state.
 * gMgGroupBase is the first record id of the visible group, gMgLastSelected
 * the last played record (-1 when none). Consumer: dScMiniGm_c.
 * ROM 0x0208a170..0x0208a178, 8 bytes, 2 configured symbols.
 */
#include "types.h"

/* 0x0208a170: 4 bytes. */
int gMgGroupBase = 0x00000003;

/* 0x0208a174: 4 bytes. */
int gMgLastSelected[1] = {
    (int)0xffffffffu,
};
