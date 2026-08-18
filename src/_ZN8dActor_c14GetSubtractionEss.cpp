//cpp
// @symbol _ZN5Actor14GetSubtractionEss
#include "Actor.h"
/* recovered: named members + shared header */

int Actor::GetSubtraction(short a, short b)
{
int d=(short)(b-a);
if(d==-0x8000) d=-0x7fff;
if(d<0) d=(short)(-d);
return d;
}
