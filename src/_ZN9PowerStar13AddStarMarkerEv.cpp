//cpp
#include "types.h"
// @symbol _ZN9PowerStar13AddStarMarkerEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "PowerStar.h"
extern void SetStarMarker(int i, int v1, int v2);
extern int IsStarCollectedInCurLevel(int starID);
extern int data_0209f40c[];
extern u8 data_0209f208;

struct Bits {
    u16 b0 : 1;
    u16 b1 : 1;
    u16 b2 : 1;
};

void PowerStar::AddStarMarker()
{
    s8 i;
    if (unk_498 >= 0) return;
    for (i = 0; i < 0xc; i++) {
        if (data_0209f40c[(int)i] != 0) continue;

        if (unk_49a == 0) {
            if (((struct Bits*)((char*)&unk_4a2))->b2) {
                SetStarMarker((int)i, (int)((char*)this), 3);
            } else {
                SetStarMarker((int)i, (int)((char*)this), 2);
            }
        } else {
            if (unk_49a == 2) {
                if (((struct Bits*)((char*)&unk_4a2))->b2) goto setmark;
                {
                    int f43c = unk_43c;
                    if (f43c == 5 || f43c == 7) {
                        if (IsStarCollectedInCurLevel(unk_49d) != 0) goto setmark;
                    }
                }
                goto skipmark;
            setmark:
                unk_49a = 3;
            skipmark:;
            }
            SetStarMarker((int)i, (int)((char*)this), unk_49a);
        }

        unk_498 = i;
        if (unk_440 == 9) {
            if (data_0209f208 == 0) return;
        }
        FUN_0202a130();
        return;
    }
}
