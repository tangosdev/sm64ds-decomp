#ifndef LIGHTBEAM_H
#define LIGHTBEAM_H

#include "dActor_c.h"
#include "Model.h"
#include "dCcAcPos_c.h"

/* The cartridge names this class daObjC1Hikari_c in RTTI. LightBeam is the
 * readable compatibility spelling already carried by every matched method.
 * Its __si_class_type_info record points directly at dActor_c, and its
 * 31-slot vtable has exactly the same extent as that base's table. */
struct LightBeam : dActor_c {
    u32 unk_0d0;                  /* 0x0d0 */
    Model mModel;                 /* 0x0d4 */
    dCcAcPos_c mCylinder;         /* 0x124 */
    dActor_c *mTalkPlayer;        /* 0x164 */
    u16 mSoundTimers[2];          /* 0x168 */

    /* Inline is load-bearing: the two forcing translation units emit the
     * genuine D1 and D0 while objisolate discards their wrappers/passengers. */
    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~LightBeam() {}   /* no slot */
#else
    virtual ~LightBeam() {}   /* D1 and D0 */
#endif

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char LightBeam_size_must_be_0x16c[
    sizeof(LightBeam) == 0x16c ? 1 : -1];
#endif

#endif
