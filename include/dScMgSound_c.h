/* class dScMgSound_c, real ROM name confirmed by tools/rtti_extract.py:
 * dScMgSound_c : dScMgSingle3DBase_c, single edge, offset 0
 * (build/rtti.json). The coined English Spawn-function name was
 * MgBoomBox_Spawn; the factory now carries the reconstructed
 * dScMgSound_c_classInit spelling, and the class itself takes its
 * real ROM identity, matching this tree's current convention.
 *
 * SIZE 0x562c, from d_s_mg_sound.cpp's own `_ZN7fBase_cnwEj(0x562c)`.
 *
 * TAIL STORAGE begins at 0x4f38 and occupies 0x6f4 bytes in this layout.
 * The factory calls func_ov006_020c33dc there and the destructor calls
 * func_ov006_020c3288 at the same address. These establish a component's
 * start and lifetime, not its full extent. The constructor builds nested
 * objects through +0x178 and writes +0x1a4/+0x1a8; those accesses do not
 * prove that the component fills the tail. Matched scene state also uses
 * 0x50e0, 0x5608, 0x5616, 0x5618 and tail bytes through 0x5628.
 * mTable retains raw storage containing the component and live state;
 * the component boundary and typed fields remain to be established under
 * issue #2492.
 *
 * THE DESTRUCTOR IS DEFINED INLINE AND DECLARED FIRST. Its class TU emits
 * _ZTV/_ZTI/_ZTS and the destructor variants. The inline form reproduces the
 * cartridge's destructor ORDER: mwccarm 2004/b56 emits D1 then D0 for an in-class body
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
    virtual void Virtual50();                          /* slot 20 */

    u8  mTable[0x6f4]; /* 0x4f38 -- component and live state; extent unresolved */

    /* --- this class's own vtable overrides, defined out of line under their
       own mangled names. Each re-uses a slot fBase_c already holds rather
       than appending one, and none adds a field, so the size assert below is
       untouched. The existing class TU retains its destructor/RTTI
       ownership; these declarations do not create a new owner.
       Signatures are include/fBase_c.h's and include/dScMgBase_c.h's own,
       copied unchanged.

       Their accesses at 0x50e0, 0x5608, 0x5616, 0x5618 and 0x5626 lie
       within the declared mTable storage. Their location alone does not
       establish whether they belong to the component or to scene state.
       Everything else they touch is inherited (unk_0b4 from dScMgBase_c,
       +0x4660 from dScMgSingle3DBase_c). --- */
    s32 InitResources();      /* slot 0 -- ov006 0x0211c984 */
    s32 Behavior();           /* slot 6 -- ov006 0x0211c720 */
    s32 Render();             /* slot 9 -- ov006 0x0211c6c4 */
};

typedef char dScMgSound_c_size_must_be_0x562c[sizeof(dScMgSound_c) == 0x562c ? 1 : -1];

#endif
