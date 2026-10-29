#ifndef DAOBJC1PEACH_C_H
#define DAOBJC1PEACH_C_H

#include "dActor_c.h"
#include "dExtSimpleModel_c.h"

/* Castle painting (profile C1_PEACH). ROM RTTI ov010:0x02112c24 names
 * daObjC1Peach_c as a direct dActor_c subclass. classInit allocates 0x128.
 * The typeinfo record points directly at dActor_c, and the 31-slot vtable
 * has the same extent as that base's table. */
struct daObjC1Peach_c : dActor_c {
    u32 unk_0d0;                 /* 0x0d0 */
    dExtSimpleModel_c mModel;    /* 0x0d4 */
    u8 mOpacity;                 /* 0x124 */
    u8 pad_125[3];

    /* Inline so InitResources can own the vtable and emit retail's D1 then
     * D0 pair without an extra D2. */
    virtual ~daObjC1Peach_c() {}

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    /* Nonvirtual helper called once by InitResources. Its body consumes this
     * actor's angle, position, scale, and embedded dExtSimpleModel_c, proving ownership. */
    void UpdateModelTransform();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjC1Peach_c_size_must_be_0x128[
    sizeof(daObjC1Peach_c) == 0x128 ? 1 : -1];
#endif

#endif
