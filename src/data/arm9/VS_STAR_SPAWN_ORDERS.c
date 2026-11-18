/* The six orders in which a course's five Power Stars spawn; each 12-byte
   row lists the star indices, then seven zero padding bytes. */
#include "types.h"

extern const char VS_STAR_SPAWN_ORDERS[6][12] = {
    {0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 3, 2, 4, 0, 0, 0, 0, 0, 0, 0},
    {0, 2, 1, 3, 4, 0, 0, 0, 0, 0, 0, 0}, {0, 2, 3, 1, 4, 0, 0, 0, 0, 0, 0, 0},
    {0, 3, 2, 1, 4, 0, 0, 0, 0, 0, 0, 0}, {0, 3, 1, 2, 4, 0, 0, 0, 0, 0, 0, 0},
};
