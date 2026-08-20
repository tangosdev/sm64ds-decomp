//cpp
// @symbol _ZN12daBakubaku_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method -- vtable slot 0
 *
 * Was a C file spelling the mangled name directly and reaching every field through
 * a raw offset. It takes one model file and two animations, builds the two body
 * cylinders from a Vector3 offset apiece, remembers where it was spawned, and hands
 * itself to the state-table installer at 0x02111ff4.
 */
#include "daBakubaku_c.h"
#include "SharedFilePtr.h"

extern "C" {
/* Three SharedFilePtrs; CleanupResources releases exactly these three. */
extern int data_ov032_02113a40[];   /* the model */
extern int data_ov032_02113a50[];   /* an animation; its +4 is the BCA */
extern int data_ov032_02113a48[];   /* an animation */
/* A Vector3 apiece: where each cylinder sits relative to the actor. */
extern int data_ov032_021137cc[];
extern int data_ov032_021137d8[];
extern void *data_ov032_02113a8c;

/* All three carry Fix12<int> parameters, so their definitions stay mangled free
   functions -- see include/ShadowModel.h. A CALL is unaffected. extern "C" so the
   reference is mangled once and not twice. */
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        dCcAcPos_c *self, dActor_c *actor, const Vector3 &offset,
        int radius, int height, u32 d, u32 e);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        dBgCh_Actr *self, dActor_c *actor, int radius, int height, void *a, void *b);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        ModelAnim *self, void *animFile, int flags, int speed, u32 startFrame);

int func_ov032_02111ff4(void *self, void *stateTable);
}

s32 daBakubaku_c::InitResources()
{
    Vector3 bodyOffset;
    Vector3 headOffset;
    void *f;

    f = Model::LoadFile(*(SharedFilePtr *)data_ov032_02113a40);
    mModelAnim.SetFile((BMD_File *)f, 1, -1);
    mShadowModel.InitCylinder();
    Animation::LoadFile(*(SharedFilePtr *)data_ov032_02113a50);
    Animation::LoadFile(*(SharedFilePtr *)data_ov032_02113a48);

    bodyOffset.x = data_ov032_021137cc[0];
    bodyOffset.y = data_ov032_021137cc[1];
    bodyOffset.z = data_ov032_021137cc[2];
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mBodyClsn, this, bodyOffset, 0x64000, 0x64000, 0x200004, 0x10);

    headOffset.x = data_ov032_021137d8[0];
    headOffset.y = data_ov032_021137d8[1];
    headOffset.z = data_ov032_021137d8[2];
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mHeadClsn, this, headOffset, 0x64000, 0x8c000, 0x200004, 0);

    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;

    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x64000, 0, 0, 0);

    mTerminalVelocity = -0x1e000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (void *)data_ov032_02113a50[1], 0, 0x1000, 0);
    mModelAnim.speed = 0x1000;

    func_ov032_02111ff4(this, &data_ov032_02113a8c);
    return 1;
}
