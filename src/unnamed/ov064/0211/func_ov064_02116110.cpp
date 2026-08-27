//cpp
// @symbol func_ov064_02116110
/* recovered: shared common types */
#include "common.h"

extern "C" {
int _ZN8dActor_c13ClosestPlayerEv(char *self);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
int _Z14ApproachLinearRsss(short &cur, short tgt, short step);
}
extern "C" int func_ov064_02116110(char *self, short step){
    char *p = (char*)(unsigned int)_ZN8dActor_c13ClosestPlayerEv(self);
    if(p != 0){
        short ang = Vec3_HorzAngle((const Vector3*)(self+0x5c),(const Vector3*)(p+0x5c));
        if(_Z14ApproachLinearRsss(*(short*)(self+0x94), ang, step)) return 1;
    }
    return 0;
}
