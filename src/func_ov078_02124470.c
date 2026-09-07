#include "types.h"
extern s16 _ZN8dActor_c18HorzAngleToCPlayerEv(void* self);
extern void ApproachAngle(void* dst, int target, int a, int b, int e);
extern int KingBobOmb_SetState(void* c, void* p);

extern int data_ov078_0212702c[];

int func_ov078_02124470(char* c)
{
    s16 ang = _ZN8dActor_c18HorzAngleToCPlayerEv(c);
    ApproachAngle(c + 0x94, ang, 1, 0x500, 0x500);
    *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
    if (*(u16*)(c + 0x100) == 0)
        KingBobOmb_SetState(c, data_ov078_0212702c);
    return 1;
}
