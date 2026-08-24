//cpp
// @symbol _ZN13BigBrickBlock13InitResourcesEv
/* BigBrickBlock::InitResources -- one class, six actor IDs. mVariant selects the
 * row of the three 0xc-stride resource tables (model, collision, CLPS) that this
 * block loads; the collision scale then varies again by ID -- 0x1800 for 0x10,
 * 0x1000 for the other variant-0/1 blocks, 0x199 for variant 2.
 *
 * Actor 0x11 is the star block: the low byte of param1 is the star index (0xff
 * meaning none) and the second byte being 0xff registers a star marker.
 * Actor 0x13 takes the event bit from the low nibble of param1 instead.
 *
 * The two tail cases return 0 -- refuse to spawn: a 0x11 block whose star is
 * already taken when the player is entering level 1, and a 0x2e block above
 * y 0xdac000 in level 7 once star 1 is collected.
 *
 * The redundant compares and the reloaded actorID are the ROM's, not
 * tidiness lost in translation: folding them changes the compare shape.
 * dBgW_KcMbg::SetFile takes Fix12<int> by value (wall 6az) so it stays an
 * extern-C mangled free function.
 */
#include "BigBrickBlock.h"
#include "SharedFilePtr.h"
#include "Model.h"


extern "C" {
extern char *data_ov002_02108ab0[];   /* model handles,     0xc stride */
extern char *data_ov002_02108ab4[];   /* collision handles, 0xc stride */
extern char *data_ov002_02108ab8[];   /* CLPS blocks,       0xc stride */
extern s8 data_0209f2f8;              /* current level */
extern u8 data_0209f220;

int _ZN6Player15IsEnteringLevelEv(Player *self);
int func_ov002_020b36a0(BigBrickBlock *self);
int IsStarCollectedInCurLevel(int star);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, void *kcl, const Matrix4x3 *mat, int scale, s16 angY, void *clps);
int _ZN5Event6GetBitEj(u32 bit);
}

int BigBrickBlock::InitResources()
{
    u8 *c = (u8 *)this;
    u16 id = actorID;
    u8 idx;

    switch (id) {
    case 0xf:
    case 0x10:
    case 0x13: mVariant = 0; break;
    case 0x11: mVariant = 1; break;
    case 0x12: break;
    case 0x2e: mVariant = 2; break;
    }

    mModel.SetFile((BMD_File *)Model::LoadFile(
        **(SharedFilePtr **)((char *)data_ov002_02108ab0 + mVariant * 0xc)), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    {
    int is11 = actorID;
    is11 = is11 == 0x11;
    if (is11 != 0) {
        int byte1;
        mStarID = *(s32 *)(c + 8);
        byte1 = ((u32)*(s32 *)(c + 8) >> 8) & 0xff;
        if (mStarID == 0xff)
            mStarID = 0;
        if (mStarID != 0 && byte1 == 0xff)
            mTrackStarID = TrackStar(mStarID, 2);
    }
    }

    idx = mVariant;
    if (idx <= 1) {
        int is10 = actorID;
        is10 = is10 == 0x10;
        if (is10 != 0) {
            void *kcl = dBgW_Kc::LoadFile(**(SharedFilePtr **)((char *)data_ov002_02108ab4 + idx * 0xc));
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                &mMeshCollider, kcl, &mClsnMat, 0x1800, mAngleY,
                *(void **)((char *)data_ov002_02108ab8 + idx * 0xc));
        } else {
            void *kcl = dBgW_Kc::LoadFile(**(SharedFilePtr **)((char *)data_ov002_02108ab4 + idx * 0xc));
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY,
                *(void **)((char *)data_ov002_02108ab8 + idx * 0xc));
        }
    } else {
        void *kcl = dBgW_Kc::LoadFile(**(SharedFilePtr **)((char *)data_ov002_02108ab4 + idx * 0xc));
        _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            &mMeshCollider, kcl, &mClsnMat, 0x199, mAngleY,
            *(void **)((char *)data_ov002_02108ab8 + idx * 0xc));
    }

    {
    int is13 = actorID;
    is13 = is13 == 0x13;
    if (is13 != 0)
        mEventID = *(s32 *)(c + 8) & 0xf;
    }
    mPrevEventBit = _ZN5Event6GetBitEj(mEventID);

    {
    Player *pl = ClosestPlayer();
    if (pl != 0 && _ZN6Player15IsEnteringLevelEv(pl) != 0) {
        int is11b = actorID;
        is11b = is11b == 0x11;
        if (is11b != false && func_ov002_020b36a0(this) != 0) {
            if (data_0209f2f8 == 1)
                return 0;
        }
    }
    }

    {
    int is2e = actorID;
    is2e = is2e == 0x2e;
    if (is2e != false && data_0209f2f8 == 7) {
        if (data_0209f220 != 1) {
            if (IsStarCollectedInCurLevel(1) != 0)
                goto ret1;
        }
        if (mPosY >= 0xdac000)
            return 0;
    }
    }

ret1:
    return 1;
}
