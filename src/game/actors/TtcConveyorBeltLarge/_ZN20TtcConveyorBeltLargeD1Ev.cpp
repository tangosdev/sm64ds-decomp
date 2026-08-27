//cpp
// @symbol _ZN20TtcConveyorBeltLargeD1Ev
/* D1, the complete-object destructor, generated from an empty body against a
 * shadow struct that names the real base and the real member types. */
#include "dBgActor_c.h"
#include "TextureTransformer.h"
#include "ShadowModel.h"

struct TtcConveyorBeltLarge : dBgActor_c {
    TextureTransformer mTexXf;          /* 0x320 */
    ShadowModel mShadowModel;           /* 0x334 */
    u8 pad_35c[0x44];
    virtual ~TtcConveyorBeltLarge();
};

typedef char TtcConveyorBeltLarge_size_must_be_0x3a0[sizeof(TtcConveyorBeltLarge) == 0x3a0 ? 1 : -1];

TtcConveyorBeltLarge::~TtcConveyorBeltLarge()
{
}
