#ifndef DAOBJKB1BILLBOARD_C_H
#define DAOBJKB1BILLBOARD_C_H

#include "Model.h"
#include "dActor_c.h"

/* ROM-proven class identity: _ZTI19daObjKb1Billboard_c and
 * _ZTS19daObjKb1Billboard_c at ov044:0x021115bc/0x021115c8. */
struct daObjKb1Billboard_c : dActor_c {
    u8    pad_0d0[0x4];
    Model mModel;          /* 0x0d4 */

    /* InitResources is the first out-of-line virtual/key function. The inline
     * destructor lets mwccarm emit the retail D1/D0 pair and class RTTI/vtable. */
    virtual ~daObjKb1Billboard_c() {}

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Render();

private:
    /* Ownership, body, call site, and codegen are proven. The readable name is
     * inferred because no original symbol string survives. */
    void UpdateModelTransform();
};

typedef char daObjKb1Billboard_c_size_must_be_0x124[
    sizeof(daObjKb1Billboard_c) == 0x124 ? 1 : -1];

#endif /* DAOBJKB1BILLBOARD_C_H */
