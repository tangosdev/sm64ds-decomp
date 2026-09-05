#ifndef DAMCFLAG_C_H
#define DAMCFLAG_C_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"

/* daMcFlag_c_classInit allocates 0x138 bytes, constructs dActor_c, writes the daMcFlag_c
 * vtable, then constructs the ModelAnim member at 0xd4.  Both destructor
 * variants destroy that member before chaining to dActor_c, independently
 * confirming the ownership and the class extent.
 *
 * The cartridge RTTI names this class daMcFlag_c: _ZTS10daMcFlag_c at ov009
 * 0x02113b6c reads "10daMcFlag_c".  The sources used to spell it Flag, a
 * coined name, and symbols.txt carried a co-address alias for the ROM vtable
 * while both spellings were live; the rename retired that alias, so ov009's
 * symbols.txt now has a single _ZTV10daMcFlag_c row at 0x02113ba0.  The
 * inheritance edge is unambiguous: daMcFlag_c derives directly from dActor_c
 * at offset zero.
 *
 * The 31-slot ROM vtable differs from dActor_c only in slots 0, 3, 6, 9, 16
 * and 17, exactly the declarations below.  All other actor virtuals are
 * inherited and must not be redeclared here.
 *
 * SM64DS RTTI names the implementation daMcFlag_c. The reconstructed factory
 * daMcFlag_c_classInit (historical alias daMcFlag_c_Spawn) installs this class's
 * cartridge vtable for the MC_FLAG registry profile.
 */
struct daMcFlag_c : dActor_c {
    u8        pad_0d0[0x4];
    ModelAnim mModelAnim;       /* 0x0d4 */

    /* INLINE, and that is load-bearing.  InitResources below is the key
       function, so the TU that defines it emits this class's vtable and RTTI;
       an out-of-line destructor would make mwccarm emit D2, D0, D1, but the
       cartridge has D1 then D0 and no D2, and production isolation orders the
       spanning delink by emission order.  See src/game/actors/d_a_mc_flag.cpp. */
    virtual ~daMcFlag_c() {}                       /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();             /* slot  0 */
    virtual s32 CleanupResources();          /* slot  3 */
    virtual s32 Behavior();                  /* slot  6 */
    virtual s32 Render();                    /* slot  9 */
};

typedef char daMcFlag_c_size_must_be_0x138[sizeof(daMcFlag_c) == 0x138 ? 1 : -1];

#endif
