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
    virtual ~PeachPainting() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    /* Nonvirtual helper called once by InitResources. Its body consumes this
     * actor's angle, position, scale, and embedded Model, proving ownership. */
    void UpdateModelTransform();
};

typedef char PeachPainting_size_must_be_0x128[
    sizeof(PeachPainting) == 0x128 ? 1 : -1];

#endif
