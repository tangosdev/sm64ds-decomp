#include "Ov007Picker.h"

/* Resets the picker to drag mode at the origin: pos, restPos, anchorPos and
 * posOffset zero, an empty bounds rectangle, no hit and no limit. */
void func_ov007_020c684c(char *o)
{
    Ov007Picker *p = (Ov007Picker *)o;
    VecFx32 zero;
    VecFx32 *tp;
    p->mode = 1;
    p->word00 = 0;
    p->pos.z = 0;
    p->pos.y = p->pos.z;
    p->pos.x = p->pos.y;
    p->posOffset.z = 0;
    p->posOffset.y = p->posOffset.z;
    p->posOffset.x = p->posOffset.y;
    p->restPos = p->pos;
    p->anchorPos = p->pos;
    tp = (VecFx32 *)(&zero);
    tp->x = 0;
    tp->y = 0;
    tp->z = 0;
    p->boundsMax = *tp;
    p->boundsMin = p->boundsMax;
    p->hitIndex = -1;
    p->hitGroup = p->hitIndex;
    p->hitOffset.z = 0;
    p->hitOffset.y = p->hitOffset.z;
    p->hitOffset.x = p->hitOffset.y;
    p->limitGroup = 0;
    p->limitIndex = 0;
}
