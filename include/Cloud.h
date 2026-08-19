#ifndef CLOUD_H
#define CLOUD_H

#include "dActor_c.h"
#include "Model.h"

/* Cloud_Spawn allocates 0x124 bytes, constructs dActor_c, stores
 * _ZTV5Cloud, then constructs a Model at 0xd4. D1 destroys that Model and
 * chains to dActor_c::~dActor_c, so both the inheritance and the closing size
 * are independently witnessed by the ROM.
 */
struct Cloud : dActor_c {
    u8    pad_0d0[0x4];
    Model mModel;            /* 0x0d4 */

    virtual ~Cloud();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

typedef char Cloud_size_must_be_0x124[
    sizeof(Cloud) == 0x124 ? 1 : -1];

#endif /* CLOUD_H */
