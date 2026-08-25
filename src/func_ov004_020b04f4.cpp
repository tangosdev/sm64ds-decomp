//cpp
// @symbol func_ov004_020b04f4
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgBase_c.h"
// recovered name: dScMgBase_c_BeforeRender
/* recovered: renamed to Class_Method */
/* dScMgBase_c::BeforeRender - recovered from vtable slot identity */
// MATCHED at 2004/b56, 1.2/base, 1.2/sp2 and 1.2/sp2p3 -- four of the 25 installed
// builds, with clean reloc destinations. This banner previously declared a non-match
// at div=17 and said the file could not be byte-matched at 1.2/sp2p3; both halves were
// STALE, and 1.2/sp2p3 is one of the four it matches at. Measured 2026-08-25 by run
// mg12 lane PANEL and re-measured by that run's independent reviewer against the tree
// state BEFORE the lane touched it, so the verdict is not an artifact of this branch.
// Nothing in the body changed to make it true; the banner simply outlived the fix.
//
// THE OLD MARKER TOKEN IS DELIBERATELY NOT QUOTED HERE. Spelling it, even inside a
// sentence retiring it, puts the literal string back in the file, and
// validate_merge.py's candidate filter is a whole-file `git grep -F` for exactly that
// token -- it would flag this genuinely matched TU as a draft candidate and rely on
// the separate 200-character test to clear it again. Right by luck is not right.
//
// THIS CORRECTION MOVES NO COUNT. The old marker sat at byte 412, past both the
// 200-character window progress.py and validate_merge.py read and the 400-character
// window nearmiss_db.py reads, so every counter in the repo ALREADY treated this
// function as matched -- which is very likely why the wrong banner survived so long.
// The correction is to the record, not to the total: +0 at both windows, measured.
typedef short s16;

struct Scene;
extern "C" int _ZN5Scene12BeforeRenderEv(struct Scene *);


struct Ent { char pad[0x1a]; s16 f; char pad2[4]; };

extern "C" int func_ov004_020b04f4(void *c)
{
    struct dScMgBase_c *self = (struct dScMgBase_c *)(void *)c;
    int i;
    char *p;
    int i2;
    char *p2;
    int j;

    if (_ZN5Scene12BeforeRenderEv((struct Scene *)c) == 0)
        return 0;

    if (self->unk_4628 != 0) {
        func_ov004_020ae858(c);
        return 0;
    }

    if (self->unk_0f0 == 0) {
        p = data_ov004_020bf648;
        for (i = 0; i < 3; i++, p += 0x134) {
            if (*(int *)(p + 0x20) == 0x1d) continue;
            if (*(s16 *)(p + 0x30) != 0) continue;
            func_ov004_020b31b4(p);
        }
        p2 = data_ov004_020bf648;
        for (i2 = 0; i2 < 3; i2++, p2 += 0x134) {
            if (*(int *)(p2 + 0x20) == 0x1d) continue;
            if (*(s16 *)(p2 + 0x30) == 0) continue;
            func_ov004_020b31b4(p2);
        }

        func_ov004_020b8714((char *)c + 0xcc);
    }

    p = data_ov004_020bebe8;
    for (j = 0; j < 0x40; j++, p += 0x20) {
        if (((struct Ent *)data_ov004_020bebe8)[j].f != 0)
            func_ov004_020add88(p);
    }

    func_ov004_020b0de0(c);
    return 1;
}



