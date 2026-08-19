#ifndef ORANGEBALLBILLBOARD_H
#define ORANGEBALLBILLBOARD_H

#include "Model.h"
#include "dActor_c.h"

/* OrangeBallBillboard_Spawn allocates 0x124 bytes, constructs dActor_c, and
 * constructs Model at 0xd4. D1 destroys that model before chaining to
 * dActor_c, closing the class layout with no additional derived state.
 */
struct OrangeBallBillboard : dActor_c {
    u8    pad_0d0[0x4];
    Model mModel;          /* 0x0d4 */

    virtual ~OrangeBallBillboard();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Render();
};

typedef char OrangeBallBillboard_size_must_be_0x124[
    sizeof(OrangeBallBillboard) == 0x124 ? 1 : -1];

#endif /* ORANGEBALLBILLBOARD_H */
