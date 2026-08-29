//cpp
// @symbol _ZN14daObjC1_Trap_c13InitResourcesEv
/* daObjC1_Trap_c::InitResources -- vtable slot 0, ov010 0x02111654.
 *
 * ONE ACTOR ID, TWO ROLES, chosen by the low byte of param1. 0xff makes this a
 * SPAWNER: it places two real traps 0x15d units either side of itself along the
 * axis its own angle picks (data_02082214 is the 16-entry sin/cos-ish table
 * indexed by mAngleY >> 4, two halfwords per entry) and stamps its own uniqueID
 * into each one's mSpawnerID so they can find it again. It loads no resources of
 * its own and returns early.
 *
 * Anything else is a real trap door: model, collision, and -- for param1 low
 * byte 1 -- a half-turn added to its angle so the pair faces opposite ways.
 *
 * The 0x8e half-turn goes through a laundered address on purpose: it is a
 * read-modify-write, where taking the member's address directly lets mwccarm CSE
 * it and costs an instruction (notes/mwccarm-codegen.md).
 *
 * THE ANGLE IS READ UNSIGNED for the table index. dActor_c types mAngleY s16,
 * but the ROM emits `ldrh r2,[r4,#0x8e]' here, not ldrsh: the index is the top
 * nibble of a 16-bit angle, where sign has no meaning and a sign-extended read
 * would pick the wrong table row for angles at or past 0x8000. The (u16) cast
 * is what restores it; without it the function misses by exactly these two
 * loads.
 *
 * dBgW_KcMbg::SetFile and dActor_c::Spawn's mangled name both carry a by-value
 * class parameter (wall 6az), so those stay extern-C free functions.
 */
#include "daObjC1_Trap_c.h"
#include "SharedFilePtr.h"
#include "Model.h"
#include "common.h"

extern "C" {
extern s16 data_02082214[];
extern SharedFilePtr data_ov010_02112d08;   /* the door BMD  */
extern SharedFilePtr data_ov010_02112d00;   /* the door KCL  */
extern char data_ov010_021122f8;            /* its CLPS block */
extern void func_ov010_02111984(void);

void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32 actorID, u32 param, const Vector3 *pos, const void *rot, int areaID, int deathTableID);
void func_ov010_0211146c(daObjC1_Trap_c *self);
void func_ov010_021113f0(daObjC1_Trap_c *self);
void func_020393c4(int *p, int v);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, void *kcl, const Matrix4x3 *mat, int scale, s16 angY, void *clps);
}

int daObjC1_Trap_c::InitResources()
{
    mTrapActive = 0;
    mSpawnerID = 0;

    if ((param1 & 0xff) == 0xff) {
        Vector3 v;
        int idx;
        int sx, sz;
        int x, y, z;
        void *spawned;

        mIsSpawner = 1;
        mPlayerDist = 0;

        idx = ((int)(u16)mAngleY >> 4) * 2;
        sx = data_02082214[idx + 1];
        sz = data_02082214[idx];
        z = mPosZ + sz * 0x15d;
        x = mPosX - sx * 0x15d;
        y = mPosY;
        v.x = x;
        v.y = y;
        v.z = z;
        spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x24, 0, &v, &mAngleX, mAreaId, -1);
        ((daObjC1_Trap_c *)spawned)->mSpawnerID = uniqueID;

        idx = ((int)(u16)mAngleY >> 4) * 2;
        sz = data_02082214[idx];
        sx = data_02082214[idx + 1];
        z = mPosZ - sz * 0x15d;
        x = sx * 0x15d + mPosX;
        y = mPosY;
        v.x = x;
        v.y = y;
        v.z = z;
        spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0x24, 1, &v, &mAngleX, mAreaId, -1);
        ((daObjC1_Trap_c *)spawned)->mSpawnerID = uniqueID;

        return 1;
    }

    mIsSpawner = 0;
    mDoorModel.SetFile((BMD_File *)Model::LoadFile(data_ov010_02112d08), 1, -1);
    func_ov010_0211146c(this);
    func_ov010_021113f0(this);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, dBgW_Kc::LoadFile(data_ov010_02112d00), &mDoorMat, 0x1000,
        mAngleY, &data_ov010_021122f8);
    func_020393c4((int *)&mMeshCollider, (int)func_ov010_02111984);
    mMeshCollider.Enable(this);
    mOpenSpeed = 0;
    mState = 0;

    if ((param1 & 0xff) == 1) {
        s16 *pAngY = (s16 *)((int)this + 0x8e);
        *pAngY = *pAngY + 0x8000;
    }
    return 1;
}
