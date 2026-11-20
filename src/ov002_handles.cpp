//cpp
/* ov002's shared file handles: shells, the Mega Mushroom, the Bob-omb, the
 * Power Flower, the water ring, the number texture and the rest. Defining
 * them here is what emits __sinit_ov002_handles.cpp. The wrapper names are
 * local; the manifest aliases their constructors and destructors to the
 * retail veneers. Definition order is the retail construction order. */

#include "SharedFilePtr.h"

struct Ov002ModelFile : SharedFilePtr {
    Ov002ModelFile(unsigned int fileID);
    ~Ov002ModelFile();
};

struct Ov002TexSeqFile : SharedFilePtr {
    Ov002TexSeqFile(unsigned int fileID);
    ~Ov002TexSeqFile();
};

Ov002ModelFile gShellGreenModelFile(0x03c7);
Ov002ModelFile gShellRedModelFile(0x03c8);
Ov002ModelFile gOneUpMushroomModelFile(0x9c0f);
Ov002ModelFile gScaleUpKinokoModelFile(0x049f);
Ov002ModelFile gBombheiModelFile(0x02b6);
Ov002ModelFile gCoinYellowPoly4ModelFile(0x8006);
Ov002ModelFile gCoinYellowPoly32ModelFile(0x8005);
Ov002ModelFile gCoinRedPoly4ModelFile(0x8008);
Ov002ModelFile gCoinRedPoly32ModelFile(0x8007);
Ov002ModelFile gCoinBluePoly4ModelFile(0x9c07);
Ov002ModelFile gCoinBluePoly32ModelFile(0x9c06);
Ov002ModelFile gFeatherModelFile(0x800c);
Ov002ModelFile gPFlowerCloseModelFile(0x0483);
Ov002ModelFile gPFlowerOpenModelFile(0x0484);
Ov002ModelFile data_ov002_0210da40(0x8011);
Ov002ModelFile data_ov002_0210d9a0(0x8010);
Ov002ModelFile data_ov002_0210d9c0(0x8014);
Ov002ModelFile data_ov002_0210d9a8(0x8009);
Ov002ModelFile gSilverNumberModelFile(0x800b);
Ov002TexSeqFile gNumberTextureSequenceFile(0x800a);
Ov002TexSeqFile gSNumberTextureSequenceFile(0x0453); /* s_number.btp, the silver digits */
Ov002ModelFile data_ov002_0210da38(0x8004);
Ov002ModelFile gWRingModelFile(0x04aa);
Ov002ModelFile data_ov002_0210d9f0(0x9c0c);

/* First function in ov002. It lives in this file so the TU has a .text section:
 * the handles' initializer is renamed from .text to .init, and a TU with no
 * ordinary function is refused. Defined after the handles so their @ counter
 * is unchanged. Death-state 8, the free-function sibling of
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
