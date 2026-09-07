/* class dScMgSound_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgSound_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). The coined English Spawn-function name was
 * MgBoomBox_Spawn; the factory now carries the reconstructed
 * dScMgSound_c_classInit spelling, and the class itself takes its
 * real ROM identity, matching this tree's current convention.
 *
 * SIZE 0x562c, from d_s_mg_sound.cpp's own `_ZN7fBase_cnwEj(0x562c)`.
 *
 * ONE OWN MEMBER, evidenced by the destructor (func_ov006_02119904,
 * pre-migration): a single explicit call to func_ov006_020c3288(c +
 * 0x4f38), mirrored by the factory's own func_ov006_020c33dc(p + 0x4f38)
 * constructor call. 0x562c - 0x4f38 = 0x6f4, so this member spans the
 * entire remainder of the object. This is a DIFFERENT ctor/dtor pair from
 * the 0x270-byte one shared by five siblings (dScMgCard_c, dScMgBSC_c,
 * dScMgMemory_c, dScMgMemory2_c, dScMgRoulette_c, dScMgMCarlo2_c) --
 * func_ov006_020c33dc/020c3288's own bodies show BlendModelAnim, Model,
 * TextureSequence, TextureTransformer and ~17 SharedFilePtr entries, a
 * larger and structurally distinct helper. No source file in this tree
 * references any offset within it, so left as raw bytes for the same
 * reason as the shared table (see include/dScMgMemory_c.h's own note) --
 * nothing to preserve, nothing to invent.
 *
 * THE DESTRUCTOR IS DEFINED INLINE, AND IS DECLARED FIRST. It is still this
 * class's key function, so the translation unit that owns it emits
 * _ZTV/_ZTI/_ZTS. The inline form is what reproduces the cartridge's
 * destructor ORDER: mwccarm 2004/b56 emits D1 then D0 for an in-class body
 * and D2/D0/D1 for an out-of-line one, and ov006 puts D1 at 0x02119904
 * BELOW D0 at 0x02119958. The body is the one the two pre-migration
 * one-function destructor shards each carried, unchanged; both are now
 * absorbed into src/actors/dScMgSound_c.cpp. No separate operator delete is
 * needed -- dScMgBase_c, two levels up, already provides one.
 *
 * SM64DS RTTI names the implementation dScMgSound_c. The reconstructed factory
 * dScMgSound_c_classInit (historical alias MgBoomBox_Spawn) installs this class's
 * cartridge vtable for the MG_SOUND registry profile.
 */
#ifndef DSCMGSOUND_C_H
#define DSCMGSOUND_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int func_ov006_020c3288(char *t); /* decl_common.h's own signature */

struct dScMgSound_c : dScMgSingle3DBase_c {
    virtual ~dScMgSound_c() { func_ov006_020c3288((char *)mTable); }
    virtual void OnYoshiTryEat(int arg);               /* slot 18 */
    virtual int  Virtual50();                          /* slot 20 */

    u8  mTable[0x6f4]; /* 0x4f38 -- ctor func_ov006_020c33dc, dtor func_ov006_020c3288 */

    /* --- this class's own vtable overrides, defined out of line under their
       own mangled names. Each re-uses a slot fBase_c already holds rather
       than appending one, and none adds a field, so the size assert below is
       untouched. The destructor above stays the key function, so no
       translation unit starts emitting _ZTV12dScMgSound_c because of these.
       Signatures are include/fBase_c.h's and include/dScMgBase_c.h's own,
       copied unchanged.

       NONE OF THE THREE ADDS A FIELD ABOVE mTable. Between them they reach
       0x50e0, 0x5608, 0x5616, 0x5618 and 0x5626, and every one of those falls
       INSIDE the 0x6f4 -- which is why the banner above can leave the member
       raw: the offsets are real matched access, but into the helper object
       func_ov006_020c33dc/020c3288 owns, not into fields of this class.
       Everything else they touch is inherited (unk_0b4 from dScMgBase_c,
       +0x4660 from dScMgSingle3DBase_c). --- */
    s32 InitResources();      /* slot 0 -- ov006 0x0211c984 */
    s32 Behavior();           /* slot 6 -- ov006 0x0211c720 */
    s32 Render();             /* slot 9 -- ov006 0x0211c6c4 */
};

typedef char dScMgSound_c_size_must_be_0x562c[sizeof(dScMgSound_c) == 0x562c ? 1 : -1];

#endif
