//cpp
// @symbol _ZN16daObjFl_London_c8BehaviorEv
#include "daObjFl_London_c.h"
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int a, void* v);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* t, int a, int b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* t);
}

s32 daObjFl_London_c::Behavior() {
    char* c = (char*)this;
    if (DecIfAbove0_Byte((unsigned char*)c + 0x31e) == 0) {
        if (mFlag == 0) {
            short* p = (short*)(c + 0x96);
            *p = *p - 0x100;
            if (mPrevAngleZ <= -0x2000) {
                mPrevAngleZ = -0x2000;
                mCooldown = 0xf;
                mFlag = 1;
            }
        } else {
            short* p = (short*)(c + 0x96);
            *p = *p + 0x100;
            if (mPrevAngleZ >= 0) {
                mPrevAngleZ = 0;
                mCooldown = 0xf;
                mFlag = 0;
            }
        }
    } else {
        if (mCooldown == 1) {
            if (mFlag == 0)
                _ZN5Sound9PlayBank3EjRK7Vector3(0x34, c + 0x74);
            else
                _ZN5Sound9PlayBank3EjRK7Vector3(0x35, c + 0x74);
        }
    }
    mAngleZ = mPrevAngleZ;
    func_ov022_02111a1c(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0))
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    return 1;
}
