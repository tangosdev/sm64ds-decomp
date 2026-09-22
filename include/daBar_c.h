#ifndef DABAR_C_H
#define DABAR_C_H

#include "dActor_c.h"
#include "dCcAc_c.h"

/* ROM-proven class identity: _ZTS7daBar_c and _ZTI7daBar_c at
 * ov002:0x02108444/0x02108450. The source-style daBar_c_classInit spelling is
 * reconstructed from that class identity and later EAD lineage; retail does
 * not preserve it. The historical project alias was InvisiblePole_Spawn.
 *
 * The factory allocates 0x108 bytes, constructs dActor_c, stores this class's
 * vptr, and constructs dCcAc_c at 0xd4. The complete destructor tears down the
 * same member and base in reverse order, closing the layout independently. */
struct daBar_c : dActor_c {
    u8 pad_0d0[0x4];       /* 0x0d0 */
    dCcAc_c mClsn;         /* 0x0d4 */

    /* InitResources is the first out-of-line virtual/key function. Together
     * with this inline destructor, mwccarm naturally emits retail D1 then D0,
     * the RTTI/vtable group, and no retained D2. */
    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~daBar_c() {}   /* no slot */
#else
    virtual ~daBar_c() {}   /* D1 and D0 */
#endif
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daBar_c_size_must_be_0x108[
    sizeof(daBar_c) == 0x108 ? 1 : -1];
#endif

/* Typed owner for the ROM descriptor at ov002:0x0210845c. The field roles are
 * established by fBase_c/dActor_c constructor consumers; their exact original
 * SM64DS member spellings are not preserved. */
struct DaBarSpawnInfo {
    daBar_c *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char DaBarSpawnInfo_size_must_be_0x1c[
    sizeof(DaBarSpawnInfo) == 0x1c ? 1 : -1];
#endif

#endif /* DABAR_C_H */
