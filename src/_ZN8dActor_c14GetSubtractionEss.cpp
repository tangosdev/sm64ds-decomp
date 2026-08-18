//cpp
// @symbol _ZN8dActor_c14GetSubtractionEss
#include "dActor_c.h"
/* recovered: named members + shared header */

int dActor_c::GetSubtraction(short a, short b)
{
int d=(short)(b-a);
if(d==-0x8000) d=-0x7fff;
if(d<0) d=(short)(-d);
return d;
}
