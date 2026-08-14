//cpp
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- the base's is out-of-line at 0x02114724 and there is
 * no bl here, so the original repeated the two statements. Every sibling's
 * slot 0 opens the same way. */
#include "cMgSmartball_dokan_c.h"

typedef struct { int x; int y; } V2;
typedef struct { V2 d; V2 v; V2 e; } W;

extern "C" void func_ov006_0211470c(int *a, int *b);
extern "C" void func_ov006_02115598(void *c, int *src, int v2, int v3, int v5);
extern "C" void func_ov006_02114fd0(char *p);
namespace Sound { void PlayBank2_2D(unsigned int); }

inline unsigned char *GetObj(char *g, int i)
{
    return i >= 13 ? 0 : *(unsigned char **)(g + 0x4688 + i * 4);
}

#pragma opt_strength_reduction off
void cMgSmartball_dokan_c::SaveSnapshot()
{
    int i;
    W s;

    mSnapshot0 = mCurrent0;
    mSnapshot1 = mCurrent1;
    for (i = 0; i < *(int *)((char *)unk_004 + 0x4668); i++) {
        if (GetObj((char *)unk_004, i)[0x30] == 0)
            continue;
        if (GetObj((char *)unk_004, i)[0x120] == 1)
            continue;
        func_ov006_0211470c((int *)&s.v, (int *)GetObj((char *)unk_004, i));
        s.d = s.v;
        s.d.x = s.d.x - mCurrent0;
        s.d.y = s.d.y - mCurrent1;
        if (s.d.x < -0x8000)
            continue;
        if (s.d.x >= 0x8000)
            continue;
        if (s.d.y < -0x40000)
            continue;
        if (s.d.y >= -0x38000)
            continue;
        s.e.x = mCurrent0;
        s.e.y = -0x40000;
        func_ov006_02115598((void *)unk_004, (int *)&s.e, 0xbb8, 0, 1);
        func_ov006_02114fd0((char *)unk_004);
        GetObj((char *)unk_004, i)[0x120] = 1;
        Sound::PlayBank2_2D(0x16);
    }
}
