//cpp
// @symbol _ZN11daTBasket_cD0Ev
#include "dEnemyBase_c.h"
#include "Model.h"
#include "dCcAc_c.h"
#include "ShadowModel.h"
#include "dBgCh_Actr.h"

struct daTBasket_c : dEnemyBase_c {
    dCcAc_c mdCcAc_c;              /* 0x110 */
    dBgCh_Actr mWithMeshClsn;      /* 0x144 */
    Model mModel;                  /* 0x300 */
    ShadowModel mShadowModel;      /* 0x350 */
    s32 mParticleID;               /* 0x378 */
    s16 mSoundTimer;               /* 0x37c */
    u8 mMuteSecretSound;           /* 0x37e */
    u8 pad_37f;

    virtual ~daTBasket_c();
};

daTBasket_c::~daTBasket_c()
{
}
