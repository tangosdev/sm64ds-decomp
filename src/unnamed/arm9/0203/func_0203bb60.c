/* Rebuild the four per-player touch records described in PlayerInput.h from
 * the sample func_0203dabc returns for each player: held, edge (old held ^ new
 * held), x and y, each truncated to a byte. */
#include "types.h"
#include "PlayerInput.h"
#include "nitro/spi/tp.h"

extern TPData *func_0203dabc(int idx);

void func_0203bb60(void)
{
    TouchInfo *p = gTouchInfo;
    int i;
    for (i = 0; i < 4; i++) {
        TPData *s = func_0203dabc(i);
        p->edge = (u8)(s->touch ^ p->held);
        p->held = (u8)s->touch;
        p->x = (u8)s->x;
        p->y = (u8)s->y;
        p++;
    }
}
