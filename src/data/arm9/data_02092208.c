/* Actor spawn table (second half) and the actor debug names.
 * data_02092208 is a 52-word table of overlay profile pointers with no
 * configured symbol names, kept as literals; 0x020922d8..0x02092654 are the
 * ACTOR_DEBUG_NAMES strings the actor dump prints.
 * ROM 0x02092208..0x020922d8, 208 bytes, 1 configured symbols.
 */
#include "types.h"

/* 52 word(s) here are addresses with no configured symbol name,
   so they stay literal rather than naming a wrong target. */

/* 0x02092208: 208 bytes. */
int data_02092208[52] = {
    0x02111210, 0x02112bdc, 0x0211229c, 0x021113ac, 0x02111c54, 0x02111844, 0x02113434, 0x02113518,
    0x02113b90, 0x02111a78, 0x02112f2c, 0x02112c68, 0x021138fc, 0x021138c4, 0x02113228, 0x02111b88,
    0x021129b4, 0x02112bec, 0x02112f58, 0x021130cc, 0x021113d0, 0x02112fd0, 0x02114ea8, 0x0211155c,
    0x02112f7c, 0x02111be0, 0x021140f8, 0x021120bc, 0x02112a6c, 0x021111ec, 0x02111218, 0x02111438,
    0x02111228, 0x0211192c, 0x02111218, 0x02111b44, 0x021113b0, 0x021124d4, 0x02111560, 0x02111a58,
    0x02111624, 0x021111fc, 0x02111204, 0x0211150c, 0x02111cc8, 0x021124f8, 0x021112b0, 0x02111994,
    0x02112c4c, 0x02111520, 0x02111768, 0x02111548,
};
