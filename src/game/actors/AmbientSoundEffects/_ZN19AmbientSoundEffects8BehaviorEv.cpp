//cpp
// @symbol _ZN19AmbientSoundEffects8BehaviorEv
#include "AmbientSoundEffects.h"
#include "Camera.h"

extern "C" {
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, void *v, u32 e);
}

extern int data_0209b4ac;
extern int data_ov002_0210b498[];
extern int data_ov002_02110aec;
extern Camera *data_0209f318;

int AmbientSoundEffects::Behavior()
{
    u32 param;
    int a;
    Camera *cam;

    if (data_ov002_02110aec != 0)
        return 1;

    param = param1;
    if (param >= 1 && param <= 4) {
        cam = data_0209f318;
        a = data_0209b4ac;
        if (a == 0x32 || a == 0x33 || a == 0x34 ||
            cam->IsUnderwater())
            return 1;
    }

    mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        mSoundHandle, 3, (u32)data_ov002_0210b498[param1],
        &mCamSpacePosX, 0);
    return 1;
}
