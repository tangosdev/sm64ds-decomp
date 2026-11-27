//cpp
/* ov002's shared file handles: shells, the Mega Mushroom, the Bob-omb, the
 * Power Flower, the water ring, the number texture and the rest. Defining
 * them here is what emits __sinit_ov002_handles.cpp. Definition order is the
 * retail construction order. */

#include "SharedFilePtr.h"

ModelFile gShellGreenModelFile(0x03c7);
ModelFile gShellRedModelFile(0x03c8);
ModelFile gOneUpMushroomModelFile(0x9c0f);
ModelFile gScaleUpKinokoModelFile(0x049f);
ModelFile gBombheiModelFile(0x02b6);
ModelFile gCoinYellowPoly4ModelFile(0x8006);
ModelFile gCoinYellowPoly32ModelFile(0x8005);
ModelFile gCoinRedPoly4ModelFile(0x8008);
ModelFile gCoinRedPoly32ModelFile(0x8007);
ModelFile gCoinBluePoly4ModelFile(0x9c07);
ModelFile gCoinBluePoly32ModelFile(0x9c06);
ModelFile gFeatherModelFile(0x800c);
ModelFile gPFlowerCloseModelFile(0x0483);
ModelFile gPFlowerOpenModelFile(0x0484);
ModelFile data_ov002_0210da40(0x8011);
ModelFile data_ov002_0210d9a0(0x8010);
ModelFile data_ov002_0210d9c0(0x8014);
ModelFile data_ov002_0210d9a8(0x8009);
ModelFile gSilverNumberModelFile(0x800b);
TexSeqFile gNumberTextureSequenceFile(0x800a);
TexSeqFile gSNumberTextureSequenceFile(0x0453); /* s_number.btp, the silver digits */
ModelFile data_ov002_0210da38(0x8004);
ModelFile gWRingModelFile(0x04aa);
ModelFile data_ov002_0210d9f0(0x9c0c);

/* First function in ov002. It lives in this file so the TU has a .text section:
 * the handles' initializer is renamed from .text to .init, and a TU with no
 * ordinary function is refused. Defined after the handles so their @ counter
 * is unchanged. Death-state 8, the sibling of
 * dEnemyBase_c::UpdateKillByInvincibleChar: the collision and the model are
 * passed in (either may be null), and the wall test is IsOnWall rather than the
 * ground-and-vertical-speed test. */
#include "dEnemyBase_c.h"
#include "dBgCh_Actr.h"
#include "common.h"
#include "dExtSimpleModel_c.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"

int dEnemyBase_c::func_ov002_020ad660(dBgCh_Actr *clsn, dExtSimpleModel_c *model, int flags) {
    Vector3 v;

    if (mDeathState != 8)
        return 0;

    if (mDeathTimer == 0 || (clsn != 0 && (clsn->IsOnGround() || clsn->IsOnWall()))) {
        if (flags & 1)
            SpawnCoin();
        if (flags & 2)
            KillAndTrackInDeathTable();
        mDeathState = 0;
        return 2;
    }

    mDeathTimer -= 1;
    UpdatePos(0);
    if (clsn != 0)
        UpdateWMClsn(*clsn, 0);

    if (model != 0) {
        mAngle.x += mSpinRateX;
        mAngle.y += mSpinRateY;
        mAngle.z += mSpinRateZ;
        Vec3_Asr(&v, &mPos, 3);
        Matrix4x3_FromTranslation(&gScratchMtx, v.x, v.y, v.z);
        Matrix4x3_ApplyInPlaceToTranslation(&gScratchMtx, 0, OnAimedAtWithEgg() >> 3, 0);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&gScratchMtx, mAngle.x, mAngle.y, mAngle.z);
        Matrix4x3_ApplyInPlaceToTranslation(&gScratchMtx, 0, (-OnAimedAtWithEgg()) >> 3, 0);
        model->mat4x3 = gScratchMtx;
    }
    return 1;
}
