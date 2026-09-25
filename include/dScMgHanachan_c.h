#ifndef DSCMGHANACHAN_C_H
#define DSCMGHANACHAN_C_H
#include "dScMgBase_c.h"

/* Which Wiggler? Up to fifteen wigglers crawl on the touch screen and one
 * of them has the star. mTimeLeft is the seconds remaining; a wrong touch
 * takes ten off.
 *
 * The name is the ROM's RTTI name, and rtti_extract.py confirms it is a
 * leaf (no RTTI record names it as a base). dScMgHanachan_c_classInit
 * (alias MgWhichWiggler_Spawn) installs its vtable for the MG_HANACHAN
 * profile.
 *
 * The destructor is non-trivial: D1 and D0 both call __cxa_vec_cleanup on
 * the 15x0x98 array at 0x4678, as dScMgBase_c's own D1/D0 do. D0's
 * deallocation comes from dScMgBase_c's operator delete. __cxa_vec_cleanup
 * is declared here, not per destructor file, like func_ov004_020b929c in
 * dScMgBase_c.h.
 */
extern "C" void __cxa_vec_cleanup(void *arr, int count, int elemSize, void *dtor);
extern "C" void *func_ov006_020ea324(void);

struct dScMgHanachan_c : dScMgBase_c {
    virtual ~dScMgHanachan_c();
    virtual s32 InitResources();         /* slot 0 */
    virtual s32 CleanupResources();      /* slot 3 */
    virtual s32 Behavior();              /* slot 6 */
    virtual s32 Render();                /* slot 9 */
    virtual void OnYoshiTryEat(int arg); /* slot 18 */

    /* Scene phase, an Itanium member pointer: Behavior calls it, Render
       compares the pair with the phase tables. */
    u32 mPhaseFn;        /* 0x4660 */
    u32 mPhaseAdj;       /* 0x4664 */
    s32 mDelay;          /* 0x4668 frames before the prompt */
    s32 mPhaseTimer;     /* 0x466c per-phase countdown: 0x4b0 at play start
                            (020ed81c), 0xb4 in 020ed300; Render and
                            020ed34c key the hit/miss beats off 0x5a/0x8a */
    s32 mTimeLeft;       /* 0x4670 seconds left */
    s32 mTimeTick;       /* 0x4674 counts down one second */
    u8  mWiggler[0xf][0x98]; /* 0x4678 */
    void *mTarget;       /* 0x4f60 the starred wiggler */
    u8  mHit;            /* 0x4f64 the star was touched */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgHanachan_c_size_must_be_0x4f68[sizeof(dScMgHanachan_c) == 0x4f68 ? 1 : -1];
#endif

#endif
