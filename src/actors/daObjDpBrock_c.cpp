//cpp
/* Production translation unit for ov025/daObjDpBrock_c.
 * 9 function(s), .text 0x02111d40..0x021120e4, the factory included.
 *
 * DP_BROCK, a pyramid step block. It sinks for 100 frames, rises for 100,
 * and repeats; param1 & 3 picks the phase it starts in. The block carries a
 * model and a collider matrix of its own besides dBgActor_c's, and two small
 * helpers keep them on the actor's position and heading.
 *
 * NAME: daObjDpBrock_c is the cartridge's RTTI spelling. _ZTS at ov025
 * 0x021138dc is the string "14daObjDpBrock_c", and the _ZTI at 0x021138d0
 * names the vtable at 0x02113914 as this class's. The tree called the class
 * PyramidStep until then.
 *
 * THE DESTRUCTOR IS INLINE AND EMPTY in the class header, so Behavior is the
 * key function and this TU emits _ZTV14daObjDpBrock_c and the RTTI chain.
 * D1 and D0 come from the header, below every function written here.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here. Do not reorder.
 *
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay mangled.
 *   Each takes Fix12<int> by value (wall 6az), and a member call homes the
 *   argument and changes the code.
 * Leftover: func_020393d4 is a 4-byte store into a dBgW callback slot;
 *   naming it belongs with dBgW in arm9.
 * Leftover: func_ov025_02111dec and func_ov025_02111e30 keep their
 *   address-derived names as C-linkage helpers taking the object as char *,
 *   the spelling include/decl_common.h gives them.
 *   Nothing in the tree spells them any other way.
 * Leftover: the model and collision files and the CLPS block are unnamed
 *   ov025 rows this TU does not own.
 */

#include "daObjDpBrock_c.h"
#include "SharedFilePtr.h"

extern "C" {
/* The block's model file and collision file (ov025 .bss), and the CLPS
   block handed to dBgW_KcMbg::SetFile. */
extern SharedFilePtr data_ov025_02113ab8;
extern SharedFilePtr data_ov025_02113ab0;
extern CLPS_Block data_ov025_02112ce8;

void Matrix4x3_FromRotationY(void *m, int angleY);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat, int scale,
    s16 angleY, CLPS_Block *clps);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void func_020393d4(int *p, int v);

/* char *, the spelling include/decl_common.h gives both helpers. */
void func_ov025_02111dec(char *c);
void func_ov025_02111e30(char *c);
}

/* -------------------------------------------------------------------------- */
// @symbol daObjDpBrock_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjDpBrock_c through RTTI,
 * allocation size, vtable identity and the DP_BROCK registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: PyramidStep_Spawn.
 *
 * Every instruction here falls out of the one `new`: operator new(932), the
 * out-of-line dBgActor_c base constructor, this class's vptr and the Model
 * constructor for mStepModel all come from the implicit constructor the
 * `new` inlines, and the null check is the one `new` itself emits. */
extern "C" daObjDpBrock_c *daObjDpBrock_c_classInit()
{
    return new daObjDpBrock_c();
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjDpBrock_c13InitResourcesEv
int daObjDpBrock_c::InitResources()
{
    mStepModel.SetFile((BMD_File *)Model::LoadFile(data_ov025_02113ab8), 1, -1);
    func_ov025_02111e30((char *)this);
    func_ov025_02111dec((char *)this);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (KCL_File *)dBgW_Kc::LoadFile(data_ov025_02113ab0),
        &mClsnMat2, 0x1000, mAngleY, &data_ov025_02112ce8);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);

    int phase = param1 & 3;
    mVertSpeed = -0x5000;
    mState = 0;
    mStateTimer = 0;
    switch (phase) {
    case 0:
        break;
    case 1:
        /* Halfway down, halfway through the sink. */
        mPosY -= 0xfa000;
        mStateTimer += 50;
        break;
    case 2:
        /* At the bottom, about to rise. */
        mPosY -= 0x1f4000;
        mState = 1;
        mVertSpeed = 0x5000;
        break;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjDpBrock_c8BehaviorEv
int daObjDpBrock_c::Behavior()
{
    switch (mState) {
    case 0:
        if (mStateTimer == 100) {
            mState = 1;
            mVertSpeed = 0x5000;
            mStateTimer = 0;
        }
        break;
    case 1:
        if (mStateTimer == 100) {
            mState = 0;
            mVertSpeed = -0x5000;
            mStateTimer = 0;
        }
        break;
    }
    mStateTimer++;
    mPosY += mVertSpeed;
    func_ov025_02111e30((char *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        func_ov025_02111dec((char *)this);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjDpBrock_c6RenderEv
int daObjDpBrock_c::Render()
{
    mStepModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol _ZN14daObjDpBrock_c16CleanupResourcesEv
int daObjDpBrock_c::CleanupResources()
{
    mMeshCollider.Disable();
    data_ov025_02113ab8.Release();
    data_ov025_02113ab0.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov025_02111e30
/* Puts the block's own model on the actor: heading from mAngleY, and the
 * position scaled down by 8 into model space. dBgActor_c's
 * UpdateModelPosAndRotY does the same for the inherited model. */
extern "C" void func_ov025_02111e30(char *c)
{
    daObjDpBrock_c *self = (daObjDpBrock_c *)c;
    Matrix4x3_FromRotationY(&self->mStepModel.mat4x3, self->mAngleY);
    self->mStepModel.mat4x3.m[9]  = self->mPosX >> 3;
    self->mStepModel.mat4x3.m[10] = self->mPosY >> 3;
    self->mStepModel.mat4x3.m[11] = self->mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
// @symbol func_ov025_02111dec
/* Moves the collider with the block: rotation from mAngleY, translation
 * from the actor position, then dBgW_KcMbg::Transform. */
extern "C" void func_ov025_02111dec(char *c)
{
    daObjDpBrock_c *self = (daObjDpBrock_c *)c;
    Matrix4x3_FromRotationY(&self->mClsnMat2, self->mAngleY);
    self->mClsnMat2.m[9]  = self->mPosX;
    self->mClsnMat2.m[10] = self->mPosY;
    self->mClsnMat2.m[11] = self->mPosZ;
    self->mMeshCollider.Transform(self->mClsnMat2, self->mAngleY);
}

/* -------------------------------------------------------------------------- */
/* D1 (0x02111d40) and D0 (0x02111d8c) have no text here. include/
 * daObjDpBrock_c.h defines ~daObjDpBrock_c() in the class body, and that
 * alone makes mwccarm emit the pair last, in the cartridge's D1-then-D0
 * order, with no D2. */
// @symbol _ZN14daObjDpBrock_cD1Ev
// @symbol _ZN14daObjDpBrock_cD0Ev
