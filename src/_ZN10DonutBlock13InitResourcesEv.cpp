//cpp
// @symbol _ZN10DonutBlock13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Load the model and the collision mesh out of this overlay's file table, put
 * both where the actor is, and pick the spin rate.
 *
 * The three raw offsets are gone: `this + 0xd4` is Platform::mModel,
 * `this + 0x124` is Platform::mMeshCollider, and the `this + 0x2ec` handed to
 * SetFile as its matrix is Platform::mClsnMat -- which UpdateClsnPosAndRot
 * filled in on the line above. A local `typedef int Fix12` went with them; the
 * real Fix12 is a template.
 *
 * unk_31e is the per-frame yaw step. It defaults to -0x80 and takes mAngleZ
 * instead when the level author set one, so the field is reused as a
 * placement parameter at load time.
 *
 * The calls keep their mangled spellings: SetFile's name carries Fix12<int> by
 * value (wall 6az), and func_020393d4 and the callback it is handed are still
 * unnamed.
 */
#include "DonutBlock.h"

extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *ptr);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *ptr);
extern int _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *file, void *mat, int scale, short angY, void *clps);
extern int func_020393d4(void *self, void *callback);
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);
}

extern void *data_ov036_02113d78[];

int DonutBlock::InitResources()
{
    void *mdl = _ZN5Model8LoadFileER13SharedFilePtr(data_ov036_02113d78[0]);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, mdl, 1, -1);
    _ZN8Platform21UpdateModelPosAndRotYEv(this);
    _ZN8Platform19UpdateClsnPosAndRotEv(this);

    void *kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov036_02113d78[1]);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, data_ov036_02113d78[2]);
    func_020393d4(&mMeshCollider,
                  &_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);

    unk_31e = -0x80;
    if (mAngleZ != 0)
        unk_31e = mAngleZ;
    return 1;
}
