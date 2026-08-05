//cpp
#include "types.h"
// @symbol _ZN19AmbientSoundEffects8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Camera.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "AmbientSoundEffects.h"
extern "C" {
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, void *v, u32 e);
}
extern void *data_0209f318;

int AmbientSoundEffects::Behavior()
{
    u32 param;
    int a;
    void *cam;

    if (data_ov002_02110aec != 0)
        return 1;

    param = unk_008;
    if (param >= 1 && param <= 4) {
        cam = data_0209f318;
        a = data_0209b4ac;
        if (a == 0x32 || a == 0x33 || a == 0x34 ||
            _ZNK6Camera12IsUnderwaterEv(cam))
            return 1;
    }

    unk_0d4 = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        unk_0d4, 3, (u32)data_ov002_0210b498[unk_008],
        ((char *)this) + 0x74, 0);
    return 1;
}
