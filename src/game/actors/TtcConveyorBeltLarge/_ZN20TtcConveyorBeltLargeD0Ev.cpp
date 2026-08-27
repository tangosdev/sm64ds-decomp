//cpp
// @symbol _ZN20TtcConveyorBeltLargeD0Ev
/* D0, the deleting destructor. Same shadow as the D1 file beside this one; one
 * destructor definition emits D0/D1/D2 and objisolate keeps the variant this
 * file's delinks entry names. */
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
