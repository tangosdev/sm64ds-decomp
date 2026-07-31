// @symbol _ZN8Particle7Texture12AllocTexVramEjb
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: named members + shared header */
#include "Particle__Texture.h"
typedef unsigned int u32;



u32 _ZN8Particle7Texture12AllocTexVramEjb(u32 size, int isTexel4x4) {
    if (isTexel4x4) {
        u32 old = unk_0209ee88;
        unk_0209ee88 = old + size;
        return old;
    } else {
        return _ZN5Model13GetVramOffsetEj(size);
    }
}
