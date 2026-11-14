/* Rebuild the four per-player touch records described in PlayerInput.h from
 * the sample func_0203dabc returns for each player: held, edge (old held ^ new
 * held), x and y, each truncated to a byte. */
#include "types.h"
#include "PlayerInput.h"
#include "nitro/spi/tp.h"

extern TPData *_Z13func_0203dabci(int idx);

void func_0203bb60(void)
{
    u8 *p = gTouchHeld;
    int i;
    for (i = 0; i < 4; i++) {
        TPData *s = _Z13func_0203dabci(i);
        p[1] = (u8)(s->touch ^ p[0]);
        p[0] = (u8)s->touch;
        p[2] = (u8)s->x;
        p[3] = (u8)s->y;
        p += 4;
    }
}
