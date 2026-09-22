#ifndef PEACHPAINTING_H
#define PEACHPAINTING_H

#include "dActor_c.h"
#include "Model.h"

/* The cartridge RTTI names this class daObjC1Peach_c. PeachPainting is the
 * readable compatibility spelling already carried by every known virtual.
 * The __si_class_type_info record points directly at dActor_c, and the class's
 * 31-slot vtable has the same extent as that base's table. */
struct PeachPainting : dActor_c {
    u32 unk_0d0;                 /* 0x0d0 */
    Model mModel;                /* 0x0d4 */
    u8 mOpacity;                 /* 0x124 */
    u8 pad_125[3];

    /* Inline is load-bearing: the forcing translation units emit the genuine
     * D1 and D0 while objisolate discards their wrapper/passenger sections. */
    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~PeachPainting() {}   /* no slot */
#else
    virtual ~PeachPainting() {}   /* D1 and D0 */
#endif

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    /* Nonvirtual helper called once by InitResources. Its body consumes this
     * actor's angle, position, scale, and embedded Model, proving ownership. */
    void UpdateModelTransform();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char PeachPainting_size_must_be_0x128[
    sizeof(PeachPainting) == 0x128 ? 1 : -1];
#endif

#endif
