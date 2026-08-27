//cpp
#include "types.h"
// @symbol _ZN10PyramidTop8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PyramidTop.h"
extern "C" {
extern int _ZN5Sound15PlaySecretSoundEP8dActor_cPt(void* actor, void* pt);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int bank, void* pos);
}

int PyramidTop::Behavior()
{
    u8 state = mState;
    switch (state) {
    case 0:
        if (mNumTagsTriggered == 4) {
            (*(u8*)((char*)&mState))++;
        }
        break;
    case 1:
        _ZN5Sound15PlaySecretSoundEP8dActor_cPt(((char*)this), (void*)((char*)&mSoundTimer));
        if (mStateTimer == 0) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x4b, (void*)((char*)&mCamSpacePosX));
        }
        func_ov024_02111350(((char*)this));
        break;
    case 2:
        if (_ZN5Sound15PlaySecretSoundEP8dActor_cPt(((char*)this), (void*)((char*)&mSoundTimer))) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x4c, (void*)((char*)&mCamSpacePosX));
            func_ov024_021112c0(((char*)this));
        } else {
            func_ov024_02111350(((char*)this));
        }
        break;
    }
    (*(u16*)((char*)&mStateTimer))++;
    if (state != mState) {
        mStateTimer = 0;
    }
    func_ov024_021114c4(((char*)this));
    func_ov024_02111480(((char*)this));
    return 1;
}
