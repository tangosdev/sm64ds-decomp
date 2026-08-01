#include "types.h"
// @symbol _ZN5Scene14StartSceneFadeEjjt
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Scene.h"
/* recovered: named members + shared header */
#include "Scene.h"
struct FaderColor {
    int vtable_dummy; // 0x0
    int currInterp;   // 0x4
    int speed;        // 0x8
    u16 color;        // 0xc
};

extern struct FaderColor data_0209f5e8;

void _ZN5Scene14StartSceneFadeEjjt(u32 actorID, u32 param, u16 fadeColor) {
    if (_ZN5Scene15SetSceneToSpawnEjj(actorID, param)) {
        data_0209f5e8.color = fadeColor;
    }
}
