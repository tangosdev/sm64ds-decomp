//cpp
#include "common.h"
#include "dMg3DEspModel_c.h"

extern Matrix4x3 data_ov006_0213c85c;

// @symbol _ZN15dMg3DEspModel_c14ResetTransformEv
void dMg3DEspModel_c::ResetTransform()
{
    *(Matrix4x3*)((char*)this + 0x28) = data_ov006_0213c85c;
}
