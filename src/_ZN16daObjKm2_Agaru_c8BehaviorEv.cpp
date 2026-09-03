//cpp
// @symbol _ZN16daObjKm2_Agaru_c8BehaviorEv
#include "daObjKm2_Agaru_c.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
// recovered name: daObjKm2_Agaru_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjKm2_Agaru_c::Behavior - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
extern "C" {
extern int _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *c);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *c, int a, int b);
extern int _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *c);
}

s32 daObjKm2_Agaru_c::Behavior() {
    char * c = (char *)this;
    struct daObjKm2_Agaru_c *self = (struct daObjKm2_Agaru_c *)(void *)c;
    switch (self->mState) {
    case 0:
        if (self->mRidden != 0)
            *(unsigned char*)(((int)c + 0x327)) =
                *(unsigned char*)(((int)c + 0x327)) + 1;
        break;
    case 1:
        if (self->mDelayTimer >= 0x14) {
            int lim;
            *(int*)(((int)c + 0x60)) =
                *(int*)(((int)c + 0x60)) + 0xa000;
            lim = self->mRestY + 0x5dc000;
            if (self->mPosY >= lim) {
                self->mPosY = lim;
                *(unsigned char*)(((int)c + 0x327)) =
                    *(unsigned char*)(((int)c + 0x327)) + 1;
                self->mDelayTimer = 0;
            }
        } else {
            *(unsigned short*)(((int)c + 0x324)) =
                *(unsigned short*)(((int)c + 0x324)) + 1;
        }
        break;
    case 2:
        if (self->mDelayTimer >= 0x14) {
            int lim;
            *(int*)(((int)c + 0x60)) =
                *(int*)(((int)c + 0x60)) - 0xa000;
            lim = self->mRestY;
            if (self->mPosY <= lim) {
                self->mPosY = lim;
                self->mState = 0;
                self->mDelayTimer = 0;
            }
        } else {
            *(unsigned short*)(((int)c + 0x324)) =
                *(unsigned short*)(((int)c + 0x324)) + 1;
        }
        break;
    }
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0x1f4000, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    self->mRidden = 0;
    return 1;
}
