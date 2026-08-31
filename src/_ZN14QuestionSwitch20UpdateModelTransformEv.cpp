//cpp
// @symbol _ZN19daObjHatenaSwitch_c20UpdateModelTransformEv
/* Inferred descriptive name. Both callers are daObjHatenaSwitch_c methods; the
 * routine rebuilds its ModelAnim matrix and mirrors it into the inherited
 * render model. */
#include "QuestionSwitch.h"

extern "C" {
void Vec3_Asr(void *dst, const void *src, int shift);
void Matrix4x3_FromTranslation(Matrix4x3 *mat, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3 *mat, s16 angleY);
extern Matrix4x3 data_020a0e68;
}
struct daObjHatenaSwitch_cMatrixCopy {
    s32 words[12];
};

void daObjHatenaSwitch_c::UpdateModelTransform()
{
    s32 shifted[4];
    Vec3_Asr(&shifted, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68,
                              shifted[0], shifted[1], shifted[2]);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    *(daObjHatenaSwitch_cMatrixCopy *)&mModelAnim.mat4x3 =
        *(daObjHatenaSwitch_cMatrixCopy *)&data_020a0e68;
    *(daObjHatenaSwitch_cMatrixCopy *)&mModel.mat4x3 =
        *(daObjHatenaSwitch_cMatrixCopy *)&mModelAnim.mat4x3;
}
