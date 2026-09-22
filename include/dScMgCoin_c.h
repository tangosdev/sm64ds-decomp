#ifndef DSCMGCOIN_C_H
#define DSCMGCOIN_C_H
#include "dScMgBase_c.h"

/* dScMgCoin_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py (no
   RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (OnYoshiTryEat -- this one
   IS genuinely named; the tree-wide OnYoshiTryEat mislabel on this class
   landed on slot 17/D0 instead, see its own source comment). Fields below
   dScMgBase_c's own 0x4660 are INHERITED, not this class's own -- accessed
   via raw offsets on a char* cast of `this` (0xac is inside dScMgBase_c's
   own pad_0ac, not a named field, so it stays raw even though 0xa4/0xa8 are
   named and used directly).
 *
 * SM64DS RTTI names the implementation dScMgCoin_c. The reconstructed factory
 * dScMgCoin_c_classInit (historical alias MgCoincentration_Spawn) installs this class's
 * cartridge vtable for the MG_COIN registry profile.
 */
struct dScMgCoin_c : dScMgBase_c {
    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~dScMgCoin_c();   /* no slot */
#else
    virtual ~dScMgCoin_c();   /* D1 and D0 */
#endif
    virtual s32 InitResources();   /* slot 0 */
    virtual s32 Behavior();        /* slot 6 */
    virtual s32 Render();          /* slot 9 */
    virtual void OnYoshiTryEat(int arg);   /* slot 18 -- signature corrected; see dScMgBase_c.h */

    u8  pad_4660[0xb68];
    s32 unk_51c8;            /* 0x51c8 */
    u8  pad_51cc[0x8];
    s32 unk_51d4;            /* 0x51d4 */
    u8  pad_51d8[0x2];
    u8  unk_51da;            /* 0x51da */
    u8  unk_51db;            /* 0x51db */
    u8  unk_51dc;            /* 0x51dc */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgCoin_c_size_must_be_0x51e0[sizeof(dScMgCoin_c) == 0x51e0 ? 1 : -1];
#endif

#endif
