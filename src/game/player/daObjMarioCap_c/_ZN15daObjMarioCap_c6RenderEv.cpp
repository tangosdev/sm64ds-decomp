//cpp
// @symbol _ZN15daObjMarioCap_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daObjMarioCap_c.h"
// _ZN15daObjMarioCap_c6RenderEv at 0x020b83c4
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" int _ZN5Model6RenderEPK7Vector3(void *, const void *);
extern "C" int data_ov002_0210df54;

int daObjMarioCap_c::Render()
{
    int b = (mFlags & 0x40000) ? 1 : 0;
    if (b) return 1;
    if (unk_3ff == 1 || mScaleX < 0x100) return 1;

    int t = mType;
    if (t == 4 || t == 17 || t == 5 || t == 18 || t == 16 || t == 11 || (unsigned)(t - 6) <= 3) {
        if (!(unk_3ec & 1) || unk_3bc == (int)&data_ov002_0210df54) {
            _ZN5Model6RenderEPK7Vector3(((char *)this) + 0x300, ((char *)this) + 0x80);
        }
    } else {
        _ZN5Model6RenderEPK7Vector3(((char *)this) + 0x300, ((char *)this) + 0x80);
    }
    return 1;
}
