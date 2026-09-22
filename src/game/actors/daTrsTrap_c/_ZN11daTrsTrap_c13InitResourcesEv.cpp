//cpp
// @symbol _ZN11daTrsTrap_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: daTrsTrap_c::InitResources() -- vtable slot 0, the setup for the
 * mansion's tilting/rotating step trap. Loads the model and its collision
 * mesh, seats the dBgW collision block, copies the placement flags out of the
 * spawn parameters, and then spawns the three step actors along the trap using
 * the offset table at data_ov063_0211e9f8.
 *
 * Two shapes in the spawn loop's preheader are load-bearing for the byte
 * match, not style (notes/mwccarm-codegen.md 6v):
 *   - the offset table is referenced by SYMBOL at the use site, never through
 *     a named local. Naming it fixes its colour but makes LICM hoist it to the
 *     front of the preheader; unnamed, LICM hoists it in first-use order and
 *     lands it where the ROM has it (after "odd = 1", before "add fp, sp").
 *   - v16's three fields are written x, y, z in that order; any other order
 *     swaps the two strh at +0x29c/+0x2a0.
 *
 * The offsets still spelled raw off `c` are the ones daTrsTrap_c.h carries as
 * padding -- 0x128/0x12c-0x134/0x138-0x13c/0x148/0x14e/0x151-0x155/0x157/0x158.
 * Every offset the header does name is a member here. dBgW_KcMbg::SetFile keeps
 * its extern-C spelling on the header's own instruction: its signature carries
 * Fix12<int> (wall 6az), so include/dBgW_KcMbg.h holds it back from being a
 * method call for now.
 */
#include "daTrsTrap_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern "C" {
void func_ov063_0211c6f8(char *c);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *f, void *m, int fx, short s, void *b);
void func_020393d4(int *p, int v);
void func_020393c4(int *p, int v);
void func_ov063_0211d28c(void);
extern int data_ov063_0211e9e8[];
extern int data_ov063_0211e9f8[];
}

int daTrsTrap_c::InitResources()
{
    char *c = (char *)this;
    int is41;
    int t;
    u16 actorType;
    s32 param;
    s32 dx;
    int i;
    int bit;
    Vector3 pos;
    void *sp;

    param = param1;
    *(u8 *)(c + 0x151) = ((u32)param >> 8) & 3;
    *(s32 *)(c + 0x128) = 0;
    is41 = 1;
    mVisible = is41;
    actorType = actorID;
    if (actorType != 0x41)
        is41 = 0;
    if (is41 != false) {
        mIndex = 0;
        if (*(u8 *)(c + 0x151) == 0) {
            bit = param1 & 1;
            dx = (bit ^ 1) * 0xc8000;
            if (bit != false)
                mVisible = 0;
            mPosY = mPosY - 0x258000 + dx;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            for (i = 1; i <= 2; i++) {
                pos.z -= 0xc8000;
                pos.y += dx;
                sp = dActor_c::Spawn(0x41, 0x100 * i, pos, 0, mAreaId, -1);
                *(s32 *)((char *)sp + 0x128) = uniqueID;
                *(u8 *)((char *)sp + 0x156) = mVisible;
            }
        }
    } else {
        t = (int)(actorType == 0xa0);
        if (t != false) {
            mIndex = 1;
        } else {
            t = (int)(actorType == 0x9e);
            if (t != false) {
                mIndex = 2;
            } else {
                mIndex = 3;
            }
        }
    }

    *(s32 *)(c + 0x12c) = mPosX;
    *(s32 *)(c + 0x130) = mPosY;
    *(s32 *)(c + 0x134) = mPosZ;
    mState = 0;
    unk_124 = 0;
    *(u8 *)(c + 0x152) = 0;
    *(u8 *)(c + 0x153) = 0;
    *(u8 *)(c + 0x154) = 0;
    *(u8 *)(c + 0x155) = 0;
    *(s32 *)(c + 0x148) = 0;
    *(s16 *)(c + 0x14e) = 0;

    dBgW_Kc::LoadFile(*(SharedFilePtr *)(void *)data_ov063_0211e28c[mIndex]);
    if (mModel.SetFile((BMD_File *)Model::LoadFile(
            *(SharedFilePtr *)(void *)data_ov063_0211e27c[mIndex]), 1, -1) == 0)
        return 0;

    func_ov063_0211c6f8(c);

    {
        int m = mIndex;
        if (m == 3) {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                (void *)&mMovingMeshCollider, *(void **)((char *)data_ov063_0211e28c[m] + 4), (void *)&mClsnMat,
                0x1000, mAngleY, (void *)data_ov063_0211e9e8[m]);
        } else {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                (void *)&mMovingMeshCollider, *(void **)((char *)data_ov063_0211e28c[m] + 4), (void *)&mClsnMat,
                0x199, mAngleY, (void *)data_ov063_0211e9e8[m]);
        }
    }

    func_020393d4((int *)&mMovingMeshCollider, (int)&dBgW::UpdatePosAndAngs);
    func_020393c4((int *)&mMovingMeshCollider, (int)&func_ov063_0211d28c);
    ((dBgW *)&mMovingMeshCollider)->Enable(this);

    if (mIndex == 2) {
        Vector3 base; Vector3_16 v16;
        int j, even, odd, actorId, neg1;
        base.x = *(s32 *)(c + 0x12c);
        base.y = *(s32 *)(c + 0x130);
        j = 0;
        base.z = *(s32 *)(c + 0x134);
        v16.x = 0;
        even = j;
        v16.y = -0x8000;
        v16.z = 0;
        odd = 1;
        actorId = 0xd5;
        neg1 = -1;
        for (; j < 3; j++) {
            base.x = *(s32 *)(c + 0x12c) - data_ov063_0211e9f8[even];
            base.y = *(s32 *)(c + 0x130) + data_ov063_0211e9f8[odd];
            base.z = *(s32 *)(c + 0x134) - 0xb4000;
            sp = dActor_c::Spawn(actorId, j, base, &v16, mAreaId, neg1);
            *(s32 *)((char *)sp + 0x418) = uniqueID;
            even += 2; odd += 2;
        }
        *(u8 *)(c + 0x158) = mAreaId;
    }
    *(u8 *)(c + 0x157) = 0;
    *(s16 *)(c + 0x138) = 0;
    *(s16 *)(c + 0x13a) = 0;
    *(s16 *)(c + 0x13c) = 0;
    return 1;
}
