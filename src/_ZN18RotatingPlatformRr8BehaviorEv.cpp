//cpp
#include "types.h"
// @symbol _ZN18RotatingPlatformRr8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingPlatformRr.h"
extern s16 data_02082214[];
extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void* v);
}

int RotatingPlatformRr::Behavior()
{
    s16 a = unk_116 << 10;
    if (unk_118 == 0) {
        int idx = ((u16)a >> 4) * 2;
        unk_08e = unk_112 + (int)((((s64)data_02082214[idx] << 13) + 0x800) >> 12);
        unk_08c = unk_110 + (int)((((s64)data_02082214[idx + 1] << 11) + 0x800) >> 12);
    } else {
        int idx = ((u16)a >> 4) * 2;
        unk_08e = unk_112 - (int)((((s64)data_02082214[idx] << 13) + 0x800) >> 12);
        unk_08c = unk_110 + (int)((((s64)data_02082214[idx + 1] << 11) + 0x800) >> 12);
    }
    {
        u16 *p = (u16 *)(((int)((char*)this) + 0x116));
        *p = *p + 1;
    }
    if (unk_119 != 0) {
        if (unk_116 == 0x40) {
            _ZN5Sound9PlayBank3EjRK7Vector3(0x75, ((char*)this) + 0x74);
            unk_116 = 0;
        }
    }
    func_ov036_02111618(((char*)this));
    return 1;
}
