//cpp
/* Production translation unit for ov091/daObjPile_c.
 * 10 function(s), .text 0x021333fc..0x02133938. The PILE stump: a ground
 * pound sinks it one step, a mega hit sinks the rest, and the last step
 * drops coins.
 *
 * NAME: _ZTS11daObjPile_c is "11daObjPile_c" at ov091 0x02135288; _ZTI at
 * 0x0213527c reads [__si_class_type_info, that string, _ZTI10dBgActor_c].
 * The tree previously called the class Stump (coined; vtable address only).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x021333fc), D0
 * (0x02133440), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is ROM-ascending.
 * The factory daObjPile_c_classInit (0x02133938) sits just past this run's
 * right edge and stays a one-function source.
 *
 * Leftover: dBgActor_c::IsClsnInRangeOnScreen, dActor_c::SpawnCoins and
 *   dBgW_KcMbg::SetFile take Fix12<int> by value, so they stay mangled.
 */

#include "decl_common.h"
#include "daObjPile_c.h"
#include "Player.h"
#include "Sound.h"
#include "SharedFilePtr.h"

/* SpawnCoins is declared on dActor_c as const Vector3 &, which is not the
 * call Behavior emits. V3's copy is what matches that call. */
struct V3 {
    int x, y, z;
    V3() {}
    V3(const V3 &o) { x = o.x; y = o.y; z = o.z; }
    ~V3() {}
};

extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(void *c, int a, int b);
extern void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *c, V3 v, unsigned int n, int f, short s);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern int func_02012694(int a, void *b);
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *c);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *c);
extern int _ZN8dActor_c18GetBitInDeathTableEv(void *c);
extern void _ZN8dActor_c17TrackInDeathTableEv(void *c);
}

/* ROM order. The out-of-line destructor is the key function, so it stays
 * first: D1, then D0, and the unused D2 is deadstripped. */
#pragma defer_codegen off

// @symbol _ZN11daObjPile_cD1Ev
// @symbol _ZN11daObjPile_cD0Ev
daObjPile_c::~daObjPile_c()
{
}

// @symbol func_ov091_02133498
extern "C" void func_ov091_02133498(char *c)
{
    daObjPile_c *self = (daObjPile_c *)c;
    dActor_c *tag = self->mLinkedTag;
    if (!tag)
        return;
    if (tag->actorID == 0x140)
        *(unsigned char *)((char *)tag + 0x10c) = 1;
}

// @symbol func_ov091_021334b8
/* func_ov091_021334b8: member calls to UpdateClsnPosAndRot and a single
 * Vector3 move the relocs. The plain position copy and the externs stay. */
extern "C" void func_ov091_021334b8(char *c, int flag)
{
    if (flag != 0) {
        unsigned char n = *(unsigned char *)(c + 0x31e);
        int *p = (int *)(((int)c + 0x60));
        int m = n * 0x3c;
        int y = *p;
        *p = y - (m << 12);
        *(unsigned char *)(c + 0x31e) = 0;
    } else {
        int *p = (int *)(((int)c + 0x60));
        int y = *p;
        *p = y - 0x3c000;
        int off = 0x31e;
        unsigned char *q = (unsigned char *)(((int)c + off));
        *q = *q - 1;
    }
    *(unsigned char *)(c + 0x31f) = 0xf;
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    if (*(unsigned char *)(c + 0x31e) != 0) return;

    struct {
        int vx, vy, vz;
        int v2x, v2y, v2z;
    } st;
    st.vx = *(int *)(c + 0x5c);
    st.vy = *(int *)(c + 0x60);
    st.vz = *(int *)(c + 0x64);
    st.vy = st.vy + 0x17c000;
    if (*(unsigned char *)(c + 0x320) == 0) {
        if (_ZN8dActor_c18GetBitInDeathTableEv(c) == 0) {
            st.v2x = st.vx;
            st.v2y = st.vy;
            st.v2z = st.vz;
            typedef void (*SpawnRef)(void *, Vector3 const &, unsigned int, int, short);
            ((SpawnRef)_ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs)(
                c, *(Vector3 *)&st.v2x, 5, 0x5000, 0);
        }
    }
    _ZN8dActor_c17TrackInDeathTableEv(c);
    func_ov091_02133498(c);
}

// @symbol _ZN11daObjPile_c15OnHitByMegaCharER6Player
void daObjPile_c::OnHitByMegaChar(Player &player)
{
    if (mBusy)
        return;
    if (&player == 0)
        return;
    if (mState == 0)
        return;
    if (mAttackCooldown)
        return;
    player.IncMegaKillCount();
    PoofDust();
    func_02012694(0x62, &mCamSpacePosX);
    func_ov091_021334b8((char *)this, 1);
}

// @symbol _ZN11daObjPile_c15OnGroundPoundedER8dActor_c
void daObjPile_c::OnGroundPounded(dActor_c &other)
{
    if (&other == 0)
        return;
    if (mState == 0)
        return;
    if (mAttackCooldown)
        return;
    Sound::PlayBank3(0x62, *(const Vector3 *)&mCamSpacePosX);
    int f = 0;
    if (other.param1 == 2) goto set;
    if (*(unsigned char *)((char *)&other + 0x703) == 0) goto done;
set:
    f = 1;
done:
    func_ov091_021334b8((char *)this, f);
}

// @symbol _ZN11daObjPile_c16CleanupResourcesEv
s32 daObjPile_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)data_ov091_02135654)->Release();
    ((SharedFilePtr *)data_ov091_0213564c)->Release();
    return 1;
}

// @symbol _ZN11daObjPile_c6RenderEv
s32 daObjPile_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN11daObjPile_c8BehaviorEv
s32 daObjPile_c::Behavior()
{
    Player *player = ClosestPlayer();
    if (mState == 3
        && GetBitInDeathTable() == 0
        && player->mPosY < mPosY + 0x64000) {
        int angle = HorzAngleToCPlayer();
        if (DistToCPlayer() > 0x190000) {
            mAngleDelta = 0;
        } else {
            s16 diff = (s16)(angle - mPrevAngle);
            int val = mAngleDelta;
            int *p;
            if ((val > 0 && diff < -100) || (val < 0 && diff > 100))
                mAngleDelta = 0;
            p = &mAngleDelta;
            *p = *p + diff;
            {
                int a = mAngleDelta;
                if (a < 0)
                    a = -a;
                if (a > 0x30000 && mBusy == 0
                    && GetBitInDeathTable() == 0) {
                    V3 v;
                    v.x = mPosX;
                    v.y = mPosY;
                    v.z = mPosZ;
                    v.y += 0xc8000;
                    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, v, 5, 0x5000, 0);
                    TrackInDeathTable();
                }
            }
        }
        mPrevAngle = angle;
    }
    DecIfAbove0_Byte((unsigned char *)&mAttackCooldown);
    _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(this, 0, 0);
    return 1;
}

// @symbol _ZN11daObjPile_c13InitResourcesEv
s32 daObjPile_c::InitResources()
{
    void *m = Model::LoadFile(*(SharedFilePtr *)data_ov091_02135654);
    mModel.SetFile((BMD_File *)m, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *k = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov091_0213564c);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x199, mAngleY, data_ov002_0210d874);
    mState = 3;
    return 1;
}
