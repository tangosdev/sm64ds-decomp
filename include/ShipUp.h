/* ShipUp's matched layout and real dBgActor_c inheritance. */
#ifndef SHIPUP_H
#define SHIPUP_H
#include "dBgActor_c.h"

struct ShipUp : dBgActor_c {
    /* dBgActor_c's data ends at 0x31e and its sizeof rounds to 0x320. The ABI
       reuses that tail padding for the first derived byte, as the matched
       ShipUp methods prove. */
    u8  mModelIndex;            /* 0x31e */
    u8  pad_31f[0x1];
    u16 mBobAngle;            /* 0x320 */
    u8  pad_322[0x2];
    s32 mSoundHandle;            /* 0x324 */
    u16 unk_328;            /* 0x328 */
    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~ShipUp();   /* no slot */
#else
    virtual ~ShipUp();   /* D1 and D0 */
#endif
    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char ShipUp_size_must_be_0x32c[sizeof(struct ShipUp) == 0x32c ? 1 : -1];
#endif

#endif
