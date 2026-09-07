//cpp
// @symbol _ZN13daObjDorifu_c8BehaviorEv
#include "daObjDorifu_c.h"
/* daObjDorifu_c::Behavior -- vtable slot 6, ov002 0x020b4bfc.
 *
 * Named from the table: 0x020b4bfc is the word slot 6 of _ZTV13daObjDorifu_c
 * holds where _ZTV10dBgActor_c holds something else, so it is this class's own
 * override of the virtual fBase_c declares. include/daObjDorifu_c.h's
 * banner already attributed this address to this class -- it is the function
 * the four bytes at 0xdc8 were named from -- so only the definition was still
 * under a func_ov002_ name.
 *
 * The three collider calls stopped being `extern`-ed mangled names: a bare
 * `extern int _ZN4dBgW9IsEnabledEv(void *)` is emitted verbatim
 * from C but mangled a SECOND time from C++, which this file now is. See the
 * note at the top of include/SharedFilePtr.h. dBgW_KcMbg inherits all
 * three from dBgW, which declares them, so they are spelled as
 * calls.
 *
 * THE `(int)` LAUNDER ON &mActivePlank IS LOAD-BEARING and is kept from the
 * pre-migration file: without it mwcc common-subexpressions the field address
 * across the DecIfAbove0_Byte call and emits one instruction fewer than the
 * ROM has. */

extern "C" {
int  _ZN5Event6GetBitEj(unsigned int bit);
unsigned char DecIfAbove0_Byte(unsigned char *p);
}

s32 daObjDorifu_c::Behavior()
{
    int i;
    dBgW_KcMbg *m;
    unsigned char *p;

    switch (mState) {
    case 0:
        if (_ZN5Event6GetBitEj(mEventBit))
            mState = 1;
        break;
    case 1:
        if (DecIfAbove0_Byte(&mStepTimer) == 0) {
            p = (unsigned char *)(int)&mActivePlank;
            *p = *p - 1;
            mStepTimer = 2;
        }
        if (mActivePlank == 0) {
            mActivePlank = 0;
            mState = 2;
        }
        break;
    case 2:
        if (_ZN5Event6GetBitEj(mEventBit) == 0)
            mState = 3;
        break;
    case 3:
        if (DecIfAbove0_Byte(&mStepTimer) == 0) {
            p = (unsigned char *)(int)&mActivePlank;
            *p = *p + 1;
            mStepTimer = 2;
        }
        if (mActivePlank >= 4) {
            mActivePlank = 4;
            mState = 0;
        }
        break;
    }

    m = mPlankClsn;
    for (i = 0; i < 5; i++) {
        if (i == mActivePlank) {
            if (!m->IsEnabled())
                m->Enable(this);
        } else {
            if (m->IsEnabled())
                m->Disable();
        }
        m++;
    }
    return 1;
}
