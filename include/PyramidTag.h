#ifndef PYRAMIDTAG_H
#define PYRAMIDTAG_H

#include "dActor_c.h"
#include "dCcAc_c.h"

/* PyramidTag is the readable symbol spelling for the class whose cartridge
 * RTTI name is daObjDlPyramidDummy_c. The ROM records a single dActor_c base
 * at offset zero and exposes both class spellings at vtable 0x02113844.
 *
 * The factory allocates 0x10c bytes, constructs dActor_c, then constructs the
 * dCcAc_c at 0x0d4. Both destructor variants destroy that member before
 * chaining to dActor_c, independently proving its ownership and the extent.
 * The remaining word is the unique ID of the PyramidTop this tag reports to.
 *
 * The 31-slot ROM vtable has the same extent as dActor_c and overrides only
 * slots 0, 6, 16, and 17. The original code TU is shared with PyramidTop and
 * four PyramidTop-only helpers; this header does not claim a standalone TU. */
struct PyramidTag : dActor_c {
    u8       pad_0d0[0x4];
    dCcAc_c  mCylinder;       /* 0x0d4 -- the tag's collision cylinder */
    u32      mPyramidTopID;   /* 0x0108 -- PyramidTop unique ID */

    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~PyramidTag();   /* no slot */
#else
    virtual ~PyramidTag();   /* D1 and D0 */
#endif
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char PyramidTag_size_must_be_0x10c[
    sizeof(PyramidTag) == 0x10c ? 1 : -1];
#endif

#endif
