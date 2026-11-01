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

/* Named parameters only. mwcc's @ counter advances once per named parameter
 * this TU has seen, and a three-character node name cannot be renamed onto a
 * free one: only eight @NN under 100 are unused. These seventy push the
 * registration nodes into the four-character range. Nothing calls this. */
void ov002HandlesAt(int a00, int a01, int a02, int a03, int a04, int a05, int a06, int a07, int a08,
                    int a09, int a10, int a11, int a12, int a13, int a14, int a15, int a16, int a17,
                    int a18, int a19, int a20, int a21, int a22, int a23, int a24, int a25, int a26,
                    int a27, int a28, int a29, int a30, int a31, int a32, int a33, int a34, int a35,
                    int a36, int a37, int a38, int a39, int a40, int a41, int a42, int a43, int a44,
                    int a45, int a46, int a47, int a48, int a49, int a50, int a51, int a52, int a53,
                    int a54, int a55, int a56, int a57, int a58, int a59, int a60, int a61, int a62,
                    int a63, int a64, int a65, int a66, int a67, int a68, int a69);

Ov002ModelFile data_ov002_0210da18(0x03c7);
Ov002ModelFile data_ov002_0210da20(0x03c8);
Ov002ModelFile data_ov002_0210d9d8(0x9c0f);
Ov002ModelFile data_ov002_0210da30(0x049f);
Ov002ModelFile data_ov002_0210d9e0(0x02b6);
Ov002ModelFile data_ov002_0210da48(0x8006);
Ov002ModelFile data_ov002_0210d9b8(0x8005);
Ov002ModelFile data_ov002_0210da50(0x8008);
Ov002ModelFile data_ov002_0210d9f8(0x8007);
Ov002ModelFile data_ov002_0210da00(0x9c07);
Ov002ModelFile data_ov002_0210d9c8(0x9c06);
Ov002ModelFile data_ov002_0210da58(0x800c);
Ov002ModelFile gPFlowerCloseModelFile(0x0483);
Ov002ModelFile gPFlowerOpenModelFile(0x0484);
Ov002ModelFile data_ov002_0210da40(0x8011);
Ov002ModelFile data_ov002_0210d9a0(0x8010);
Ov002ModelFile data_ov002_0210d9c0(0x8014);
Ov002ModelFile data_ov002_0210d9a8(0x8009);
Ov002ModelFile data_ov002_0210da28(0x800b);
Ov002TexSeqFile data_ov002_0210d9e8(0x800a);
Ov002TexSeqFile data_ov002_0210da08(0x0453);
Ov002ModelFile data_ov002_0210da38(0x8004);
Ov002ModelFile data_ov002_0210da10(0x04aa);
Ov002ModelFile data_ov002_0210d9f0(0x9c0c);

/* First function in ov002. It lives in this file so the TU has a .text section:
 * the handles' initializer is renamed from .text to .init, and a TU with no
 * ordinary function is refused. Defined after the handles so their @ counter
 * is unchanged. Death-state 8, the free-function sibling of
 * dEnemyBase_c::UpdateKillByInvincibleChar: the enemy, its collision and its
 * model arrive as raw pointers, and the wall test is IsOnWall rather than the
 * ground-and-vertical-speed test. */
#include "dEnemyBase_c.h"
#include "dBgCh_Actr.h"
/* common.h before dExtSimpleModel_c.h, so Matrix4x3 stays the flat 12-word
 * spelling. The structured spelling scalarizes the copy at the end of this
 * function. */
#include "common.h"
#include "dExtSimpleModel_c.h"

extern "C" {
/* local extern: CalcLib.h / MtxStack.h spell these, and MtxStack.h spells
 * data_020a0e68 as Matrix4x3. That structured type scalarizes the copy below. */
extern void Vec3_Asr(Vector3 *dst, Vector3 *src, int sh); /* local extern: flat matrix copy */
extern void Matrix4x3_FromTranslation(void *m, int x, int y,
                                      int z); /* local extern: flat matrix copy */
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y,
                                                int z); /* local extern: flat matrix copy */
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y,
                                                   int z); /* local extern: flat matrix copy */
extern char data_020a0e68;                                 /* local extern: flat matrix copy */
}

// @symbol func_ov002_020ad660
extern "C" int func_ov002_020ad660(void *enemy, void *clsnArg, void *modelArg, int flags) {
    dEnemyBase_c *c = (dEnemyBase_c *)enemy;
    dBgCh_Actr *clsn = (dBgCh_Actr *)clsnArg;
    dExtSimpleModel_c *model = (dExtSimpleModel_c *)modelArg;
    int v[3];

    if (c->mDeathState != 8)
        return 0;

    if (c->mDeathTimer == 0 || (clsn != 0 && (clsn->IsOnGround() || clsn->IsOnWall()))) {
        if (flags & 1)
            c->SpawnCoin();
        if (flags & 2)
            c->KillAndTrackInDeathTable();
        c->mDeathState = 0;
        return 2;
    }

    c->mDeathTimer -= 1;
    c->UpdatePos(0);
    if (clsn != 0)
        c->UpdateWMClsn(*clsn, 0);

    if (model != 0) {
        c->mAngleX += c->mSpinRateX;
        c->mAngleY += c->mSpinRateY;
        c->mAngleZ += c->mSpinRateZ;
        Vec3_Asr((Vector3 *)v, &c->mPos, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, c->OnAimedAtWithEgg() >> 3, 0);
        Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, c->mAngleX, c->mAngleY, c->mAngleZ);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, (-c->OnAimedAtWithEgg()) >> 3, 0);
        model->mat4x3 = *(Matrix4x3 *)&data_020a0e68;
    }
    return 1;
}
