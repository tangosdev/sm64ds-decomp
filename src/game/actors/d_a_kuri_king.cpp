//cpp
/* daKuriKing_c -- the goomba-king boss (KURIKING / KURIKING_VANISH). ROM RTTI
 * is daKuriKing_c (_ZTI12daKuriKing_c at ov074:0x02122e30, _ZTS12daKuriKing_c
 * at 0x02122e3c); the vtable it owns is at ov074:0x02122eb8. This tree used to
 * spell the class Goomboss; the rename is recorded in the manifest's
 * class-label note.
 *
 * Out-of-line destructor first and `#pragma defer_codegen off`: that pair
 * emits D1 then D0 at the head of this TU, so source order is ROM order.
 * The definition also emits an unused D2, licensed `deadstrip`. The
 * Vector3 lifecycle helpers reuse the original ARM9 callbacks through
 * duplicate policies. This TU supplies its own native RTTI and vtable;
 * inherited metadata keeps its canonical owner. It spans ov074 .text 0x0211f000..
 * 0x0212290c and ends with the two registry factories (KURIKING_VANISH,
 * then KURIKING).
 *
 * Most members are named from their bodies and callers (tier B, see
 * symbols/actor_renames.tsv); the empty state 8 enter member 0211fc34 and the state 4
 * tick 0211ffcc keep func_ov074_* names, as do the free functions.
 * All but six take the object as arg0 and touch only daKuriKing_c state, so
 * they are declared as ordinary out-of-line members here; member form is
 * byte-identical under 2004/b56.
 *
 * State machine (data_ov074_021230f8): ChangeState sets mState and
 * runs the entry's first handler (enter); Behavior runs the second (tick)
 * every frame for the main boss (not param1 0x1111) through
 * UpdateState. The roles below are read off the
 * tick bodies.
 *   0  intro cutscene and first talk           tick 021201f0
 *   1  wait for the talk to close, boss music  tick 0212018c
 *   2  summon a wave of Goombas                enter 0211ffac, tick 0211fd74
 *   3  walk the arena ring                     enter 0212016c, tick 02120080
 *   4  player close behind: clip 5, then 3/2   tick 0211ffcc
 *   5  knocked back by a hit (mSizeIndex--)    enter 0211fd48, tick 0211fc38
 *   6  shrink one size step                    enter 0211fb84, tick 0211fa74
 *   7  defeat: talk, vanish wave and shrink,   enter 0211fa08, tick 0211f5b8
 *      burst, then spawns actor 0x11a (key)
 *   8  hit by a Mega player, slides to a stop  tick 0211fbd0
 * States 0, 1 and 4 enter through empty free stubs; state 8's enter is the
 * empty member 0211fc34.
 *
 * The compiler-numbered statics (the PMF descriptors and the static
 * initializer's registration nodes) are pinned by name in the TU manifest.
 * mwcc's counter for them advances on every named variable and named
 * parameter before them (prototypes included) and on some compiler
 * temporaries, so an edit here must keep that count unchanged or re-pin the
 * manifest's defined_symbol_renames.
 *
 * deslop leftovers:
 * - func_ov074_0212087c and func_ov074_02121270 take the object as arg1 (out
 *   param in arg0), and func_ov074_0212007c/021201ec/021203e0 are (void)
 *   stubs whose data records don't carry `this`. Those five stay free.
 * - mBodyMatrix is raw s32 words reached through a Matrix4x3 cast; a
 *   Matrix4x3 member there would run ~Vector3 from ~daKuriKing_c.
 *   mShadowMatrices is left untyped with it [GUESS: same ~Vector3 cost].
 * - State dispatch stays a pointer-to-member on a non-virtual stand-in:
 *   daKuriKing_c has virtuals, so its own member pointers are wider than the
 *   8-byte records the sinit copies in.
 * - The remaining mangled calls that carry Fix12<int> (Particle::System::
 *   NewSimple, daPly_c::Hurt, the Init family,
 *   Earthquake, DropShadowRadHeight, cstd::atan2, Sound::ChangeMusicVolume)
 *   pass it as a raw int. The header form with Fix12<int> locals builds a
 *   stack temporary and changes size (021223bc goes 0x278 -> 0x288 through
 *   DropShadowRadHeight).
 */

/* common.h BEFORE daKuriKing_c.h: with the structured Matrix4x3,
 * func_ov074_02121270/02121300 go long on whole-matrix assignment. */
/* Runtime asset handles resolved through the overlay 0 file table. */
enum daKuriKing_c_AssetId {
    ASSET_KURIKING_ANGER_BCA = 0x038f,
    ASSET_KURIKING_ANGER_BTP = 0x0390,
    ASSET_KURIKING_DAMAGE_B_BCA = 0x0391,
    ASSET_KURIKING_DAMAGE_F_BCA = 0x0392,
    ASSET_KURIKING_DOWN_BCA = 0x0393,
    ASSET_KURIKING_DOWN_START_BCA = 0x0394,
    ASSET_KURIKING_MODEL_BMD = 0x0395,
    ASSET_KURIKING_SERCH_WAIT_BCA = 0x0396,
    ASSET_KURIKING_SPAWN_BCA = 0x0397,
    ASSET_KURIKING_TURN_BCA = 0x0398,
    ASSET_KURIKING_WAIT_BCA = 0x0399,
    ASSET_KURIKING_WALK_BCA = 0x039a,
    ASSET_KURIKING_WALK_END_BCA = 0x039b,
    ASSET_KURIKING_WALK_START_BCA = 0x039c
};

#include "common.h"
#include "daKuriKing_c.h"
#include "types.h"
#include "SharedFilePtr.h"
#include "SpawnInfo.h"
#include "dExtAnmMaterial_c.h"
#include "dExtAnmTexPat_c.h"
#include "dBgCh_Gnd.h"
#include "daPly_c.h"
#include "daKrb_c.h"
#include "dCamera_c.h"
#include "Message.h"
#include "nitro/fx/fx.h"
#include "cstd.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "LevelRng.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/fx/fx_trig.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"

/* Resource handles this TU's static initializer constructs. The wrapper
 * spellings are reconstructed; constructor and destructor addresses, file
 * IDs, widths, and BSS order are the ROM's. */
struct KuriKingModelFilePtr : SharedFilePtr {
    KuriKingModelFilePtr(u32 fileID);
    ~KuriKingModelFilePtr();
};

struct KuriKingTexSeqFilePtr : SharedFilePtr {
    KuriKingTexSeqFilePtr(u32 fileID);
    ~KuriKingTexSeqFilePtr();
};

struct KuriKingAnimationFilePtr : SharedFilePtr {
    KuriKingAnimationFilePtr(u32 fileID);
    ~KuriKingAnimationFilePtr();
};

typedef void (daKuriKing_c::*KuriKingStateFn)();

/* Three state slots target free (void) stubs, not members, so those
 * descriptors are function-pointer/PMF pairs. The first word carries the
 * code relocation; the second is zero, like every other descriptor. */
union KuriKingFreeState {
    void (*fn)();
    KuriKingStateFn pmf;
};

extern dCamera_c *gCamera;
extern "C" int RandomIntInternal(int *seed);
extern "C" u16 DecIfAbove0_Short(u16* p);
s16 Vec3_VertAngle(const struct Vector3 *v1, const struct Vector3 *v0);
void func_02012694(u32 a, const Vector3 *b);

/* local extern: include/Sound.h does not declare the layer-3 pair yet. */
namespace Sound {
void StopLoadedMusic_Layer3();
void LoadAndSetMusic_Layer3(unsigned int musicId);
}
extern "C" void func_02011cfc();
namespace Sound {
void ChangeMusicVolume(u32 a, int f);
}
extern unsigned short data_ov074_02122e04[];
extern "C" int ApproachAngle(short *, short, int, int, int);
void func_ov084_021296b0(void *a, void *b);
extern "C" int __aeabi_idiv(int a, int b);
extern "C" int AngleDiff(int a, int b);
extern "C" int Math_Function_0203b14c(int* p, int a, int b, int d, int e);
extern unsigned short data_ov074_02122dfc[];
extern int data_ov074_0212291c[];
extern int data_ov074_0212290c[];
extern float data_ov074_02122e4c[];
extern "C" void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
extern "C" s16 atan2_hold_d_a_kuri_king_1(s32 y, s32 x);
extern "C" unsigned char DecIfAbove0_Byte(unsigned char *p);
extern "C" int *Vec3_LslInPlace(void *v, int n);
extern struct Matrix4x3 data_020a0e68;
extern struct Matrix4x3 IDENTITY_MATRIX4X3;
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, void *actor, s32 fa, s32 fb, void *v0, void *v1);
extern "C" void d_a_kuri_kingEarthquakeHold(void *thiz, void *v, int f);
extern int data_ov074_02122e24[];
extern char data_ov074_02122f3c[];
extern char data_ov074_02122f38[];
void UnloadKeyModels(int i);
void LoadKeyModels(int idx);
extern void *data_ov074_02122e5c[];
/* Boss-overlay resource blobs, linked into the co-resident 0x0211xxxx
   overlay; ov074's image has no copy, so these bind cross-module. */
extern BMA_File data_ov010_021123f4;
extern BTA_File data_ov010_021124ac;
extern "C" void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
extern "C" int func_02037e20(int *p);
extern "C" unsigned int func_0201277c(unsigned int a);
extern "C" u8 data_ov074_02122d80[];
extern SharedFilePtr data_ov002_0210da30;
extern SharedFilePtr data_ov084_02130cf8;
extern SharedFilePtr data_ov084_0213089c;
extern KrbAnimationFileHandle data_ov084_02130cc8;
extern KuriKingModelFilePtr gKurikingModelModelFile;
extern KuriKingTexSeqFilePtr gKurikingAngerTextureSequenceFile;
extern KuriKingAnimationFilePtr data_ov074_02123030;
extern SharedFilePtr *const sKurikingMinionAnimationFiles[7];
extern SharedFilePtr *const sKurikingAnimationFiles[12];

/* Intra-TU forward decls: calls run both ways across the file. */
void func_ov074_02121270(Vector3 *out, daKuriKing_c *self, int bone);
void func_ov074_0212087c(int *out, daKuriKing_c *self, int n_);
void func_ov074_021203e0(void);
void func_ov074_021201ec(void);
void func_ov074_0212007c(void);

/* data_ov074_021230f8 is copied as 8-byte records (__sinit_ov074). daKuriKing_c
 * has virtuals, so a daKuriKing_c::* is wider than that; the call stays on this
 * stand-in. The word it reads is mState. */
struct KuriKingPmf;
typedef void (KuriKingPmf::*KuriKingPMF)();
struct KuriKingPmfEntry { KuriKingPMF pmf[2]; };

extern KuriKingFreeState data_ov074_02122d9c;
extern KuriKingFreeState data_ov074_02122dbc;
extern KuriKingFreeState data_ov074_02122e0c;

/* The state table this TU's static initializer fills. Nine entries of two
 * PMFs, copied in slot order. The constructor is defined inline so it
 * expands directly into the initializer, like the descriptors it
 * materializes; an out-of-line copy leaves a call the ROM does not have.
 * Fifteen slots target daKuriKing_c members (named inline, the compiler
 * materializes those as temporaries); three target free stubs and stay
 * named below. */
struct KuriKingStateTable {
    KuriKingPmfEntry entries[9];
    /* Section-alignment fill: retail BSS runs 0x18 bytes past the ninth
     * entry to the section end at 0x021231a0. The initializer never touches
     * them; they are zeros. The intact claim has to span them because dsd
     * sizes the trailing symbol to the section end. */
    u8 tailFill[0x18];

    KuriKingStateTable()
    {
        entries[0].pmf[0] = reinterpret_cast<KuriKingPMF>(data_ov074_02122dbc.pmf);
        entries[0].pmf[1] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::UpdateIntroTalk);
        entries[1].pmf[0] = reinterpret_cast<KuriKingPMF>(data_ov074_02122e0c.pmf);
        entries[1].pmf[1] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::UpdateIntroTalkEnd);
        entries[2].pmf[0] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::EnterSummonGoombas);
        entries[2].pmf[1] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::UpdateSummonGoombas);
        entries[3].pmf[0] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::EnterWalk);
        entries[3].pmf[1] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::UpdateWalk);
        entries[4].pmf[0] = reinterpret_cast<KuriKingPMF>(data_ov074_02122d9c.pmf);
        entries[4].pmf[1] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::func_ov074_0211ffcc);
        entries[5].pmf[0] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::EnterKnockback);
        entries[5].pmf[1] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::UpdateKnockback);
        entries[6].pmf[0] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::EnterScaleChange);
        entries[6].pmf[1] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::UpdateScaleChange);
        entries[7].pmf[0] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::EnterDefeat);
        entries[7].pmf[1] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::UpdateDefeat);
        entries[8].pmf[0] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::func_ov074_0211fc34);
        entries[8].pmf[1] = reinterpret_cast<KuriKingPMF>(&daKuriKing_c::UpdateMegaKnockback);
    }
};

extern KuriKingStateTable data_ov074_021230f8;
struct KuriKingPmf { char pad[0x5cc]; s32 mState; };

#pragma defer_codegen off

/* Out of line, and FIRST in this ROM-ascending file: that is what puts D1 and
 * D0 at 0x0211f000 / 0x0211f0a0, the two lowest addresses in the run. */
daKuriKing_c::~daKuriKing_c() {}

void daKuriKing_c::SetDefeatTalkCamera() {
    struct Vector3 look, pos, in, out;
    dCamera_c* cam;
    cam = gCamera;
    cam->SetFlag_3();
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    look.x = this->mPosX;
    look.y = this->mPosY;
    look.z = this->mPosZ;
    pos.x = this->mPosX;
    pos.y = this->mPosY;
    pos.z = this->mPosZ;
    look.y += 0x1a0000;
    in.z = 0x59c000;
    Matrix4x3_FromRotationY(&data_020a0e68, this->mAngleY);
    MTX_MultVec43(&in, &data_020a0e68, &out);
    pos.x = pos.x + out.x;
    pos.y = pos.y + 0x300000;
    pos.z = pos.z + out.z;
    cam->SetLookAt(look);
    cam->SetPos(pos);
}

void daKuriKing_c::LaunchVanishCopies() {
    daKuriKing_c *a = (daKuriKing_c *)dActor_c::FindWithActorID(0xc7, 0);
    if (a == 0) return;
    do {
        if (a != this) {
            daKuriKing_c *t = a;
            t->mVertAccel = -0xa000;
            int rv = (unsigned int)RandomIntInternal(&data_0209e650.value) >> 8;
            t->mTimer = 0x30;
            t->mHorzSpeed = ((rv & 0xf) << 0xc) + 0x16000;
            t->mSpeedX = 0;
            t->mVertSpeed = ((rv & 0x1f) << 0xc) + 0x60000;
            t->mSpeedZ = 0;
            Vector3_16 pos = t->mPrevAngle;
            rv &= 0x1000;
            pos.y = (short)(pos.y + rv);
            short x = pos.x + 0;
            t->mPrevAngleX = x;
            short y = pos.y + 0;
            t->mPrevAngleY = y;
            short z = pos.z + 0;
            t->mPrevAngleZ = z;
            t->mAngleX = x;
            t->mAngleY = y;
            t->mAngleZ = z;
            t->mLaunched = 1;
        }
        a = (daKuriKing_c *)dActor_c::FindWithActorID(0xc7, a);
    } while (a != 0);
}

void daKuriKing_c::SetOtherCopiesShouldRender(u8 value) {
    daKuriKing_c *other = (daKuriKing_c *)dActor_c::FindWithActorID(0xc7, 0);
    while (other) {
        if (other != this)
            other->mShouldRender = value;
        other = (daKuriKing_c *)dActor_c::FindWithActorID(0xc7, other);
    }
}

/* Release one KURIKING_VANISH copy (actor 0xc7, param 0x1111, drawn with the
   ov084 Goomba model) on a timer during the defeat (state 7, substate 2 of
   0211f5b8), and report whether the whole wave has been released.  This is
   not state 2's Goomba wave (0211fd74, actor 0xc8).  The wave is built in
   three tiers: the first seven copies ring the base, the next five sit higher
   on a narrower ring, the next three higher and narrower still, and the
   sixteenth onward spawns dead centre, highest of all.  The spawn count needed is a table lookup on mSizeIndex, with one
   extra on size 1 unless the closest player's param1 reads 3. */
int daKuriKing_c::SpawnVanishCopy() {
    struct Vector3 spawnPos, basePos;
    struct Vector3_16 spawnAngle;
    int radius;
    int spawnTarget;
    daPly_c *player;
    spawnPos.x=0; spawnPos.y=0; spawnPos.z=0;
    spawnAngle.x=0; spawnAngle.y=0; spawnAngle.z=0;
    spawnPos.x=this->mPosX;
    spawnPos.y=this->mPosY;
    spawnPos.z=this->mPosZ;
    spawnPos.y+=0xe4000;
    basePos.x=spawnPos.x;
    basePos.y=spawnPos.y;
    basePos.z=spawnPos.z;
    spawnAngle.y = this->mAngleY;
    if(DecIfAbove0_Short(&this->mTimer)==0){
        u8 spawned=this->mSpawnedCount;
        if(spawned<=6){
            spawnAngle.y=(short)(spawned*0x2492+spawnAngle.y);
            radius=0x18c000;
        } else if(spawned<=0xb){
            spawnPos.y+=0xc4000;
            spawnAngle.y=(short)((this->mSpawnedCount-7)*0x3333+spawnAngle.y);
            radius=0x108000;
        } else if(spawned>0xe){
            spawnPos.y+=0x24c000;
            radius=0;
        } else {
            spawnPos.y+=0x188000;
            spawnAngle.y=(short)((this->mSpawnedCount-0xc)*0x5555+spawnAngle.y);
            radius=0x84000;
        }
        spawnPos.x += FX_MUL(radius, FX_SinIdx((u16)spawnAngle.y >> 4));
        spawnPos.z += FX_MUL(radius, FX_CosIdx((u16)spawnAngle.y >> 4));
        spawnAngle.x=Vec3_VertAngle(&basePos, &spawnPos);
        dActor_c::Spawn(0xc7, 0x1111, spawnPos, &spawnAngle, this->mAreaId, -1);
        this->mSpawnedCount += 1;
        this->mTimer = 2;
    }
    spawnTarget=data_ov074_02122d80[this->mSizeIndex];
    player=this->ClosestPlayer();
    if(this->mSizeIndex==1){ if((int)player->param1!=3) spawnTarget+=1; }
    return this->mSpawnedCount>=spawnTarget ? 1 : 0;
}

void daKuriKing_c::UpdateDefeat() {
    Vector3 vzero;
    Vector3 vmsg;
    daPly_c *player;
    s32 state;
    dCamera_c* cam;

    vzero.x = 0;
    vzero.y = 0;
    vzero.z = 0;
    vmsg.x = this->mPosX;
    vmsg.y = this->mPosY;
    vmsg.z = this->mPosZ;
    vmsg.y = vmsg.y + 0x78000;
    cam = gCamera;
    player = this->mTalkPlayer;

    if (this->mAnmModel.Finished() != 0) {
        PlayAnim(4);
    }

    switch (this->mSubState) {
    case 0:
        SetDefeatTalkCamera();
        {
            u16 msgId = (u16)((s32)player->param1 + 0xd7);
            state = player->GetTalkState();
            if (state != 0) {
                if (state == 1)
                    return;
                if (state != 2)
                    return;
            } else {
                if (player->ShowMessage(*this, (s16)msgId, &vmsg, 1, 2) == 0)
                    return;
                func_0201277c(0x15a);
                return;
            }
        }
        this->mSubState += 1;
        return;
    case 1:
        if (DecIfAbove0_Short(&this->mTimer) == 0) {
            dActor_c *actor = dActor_c::FindWithActorID(0xc8, 0);
            if (actor != 0) {
                u32 id = 0xc8;
                while (1) {
                    s32 flag = (actor->mFlags & 0x40000) ? 1 : 0;
                    if (flag == 0) {
                        Vector3 v2;
                        s32* src = &actor->mPosX;
                        v2.x = src[0];
                        v2.y = src[1];
                        v2.z = src[2];
                        this->PoofDustAt(v2);
                        actor->fBase_c::MarkForDestruction();
                        this->mTimer = 2;
                        return;
                    }
                    actor = dActor_c::FindWithActorID(id, actor);
                    if (actor == 0)
                        break;
                }
            }
        }
        if (this->mSpawnedCount == 1) {
            dActor_c *a2 = dActor_c::FindWithActorID(0xc8, 0);
            if (a2 != 0) {
                s32 flag2;
                if (a2->mFlags & 0x40000)
                    flag2 = 1;
                else
                    flag2 = 0;
                if (flag2 != 0) {
                    this->mSpawnedCount -= 1;
                }
            }
        }
        if (this->mSpawnedCount != 0)
            return;
        func_02012694(0x15c, (const Vector3 *)(&this->mCamSpacePosX));
        this->mSubState += 1;
        this->mSpawnedCount = 0;
        return;
    case 2:
        if (this->mWaveFinished == 0 && SpawnVanishCopy() != 0) {
            this->mWaveFinished = 1;
        }
        {
            s32 r4 = AdvanceMaterialChanger();
            if (StepScaleTransition() == 0)
                return;
            if (r4 == 0)
                return;
        }
        this->mSpawnedCount = 0;
        this->mSubState += 1;
        return;
    case 3:
        if (player->HasFinishedTalking() == 0)
            return;
        this->mShouldRender = 0;
        vzero.x = this->mPosX;
        vzero.y = this->mPosY;
        vzero.z = this->mPosZ;
        vzero.y = vzero.y + 0x190000;
        Particle::System::NewSimple(kPtclGoombossVanishA, vzero.x, vzero.y, vzero.z);
        Particle::System::NewSimple(kPtclGoombossVanishB, vzero.x, vzero.y, vzero.z);
        func_02012694(0x15f, (const Vector3 *)(&this->mCamSpacePosX));
        SetOtherCopiesShouldRender(1);
        LaunchVanishCopies();
        cam->mFlags &= ~8;
        Message::EndTalk();
        Sound::StopLoadedMusic_Layer3();
        func_02011cfc();
        Sound::ChangeMusicVolume(0x7f, 0x15666);
        this->mNoShadow = 1;
        this->mTimer = 0xf;
        this->mSubState += 1;
        return;
    case 4:
        vzero.x = this->mPosX;
        vzero.y = this->mPosY;
        vzero.z = this->mPosZ;
        vzero.y = vzero.y + 0x12c000;
        dActor_c::Spawn(0x11a, 2, vzero, 0, this->mAreaId, -1);
        func_02012694(0xbb, (const Vector3 *)(&this->mCamSpacePosX));
        this->fBase_c::MarkForDestruction();
        return;
    default:
        return;
    }
}

void daKuriKing_c::EnterDefeat() {
    this->mSizeIndex = 0;
    int v = data_ov074_02122e4c[1];
    this->mScaleX = v;
    this->mScaleY = v;
    this->mScaleZ = v;
    this->mTextureSequence.currFrame = 0x2000;
    this->mMaterialChanger.currFrame = ((int)data_ov074_02122e04[1]) << 0xc;
    this->mSubState = 0;
    PlayAnim(3);
    this->mScaleSteps = 3;
}

void daKuriKing_c::UpdateScaleChange() {
    /* 0x603 does not fit an ARM immediate, so the increment is an address
     * materialized from the pool. A plain mSubState++ drops that and comes
     * out 0x14 short. */
    int b;
    switch (this->mSubState) {
    case 0:
        if (IsAnimPlaying(6) != 0) {
            unsigned char* p;
            b = FinishTurnAround();
            if (b == 0) return;
            p = (unsigned char*)(((int)&this->mSubState));
            *p = *p + 1;
            PlayAnim(0);
            func_02012694(0x15c, (const Vector3 *)(&this->mCamSpacePosX));
            return;
        }
        {
            unsigned char* p = (unsigned char*)(((int)&this->mSubState));
            *p = *p + 1;
        }
        PlayAnim(0);
        func_02012694(0x15c, (const Vector3 *)(&this->mCamSpacePosX));
        return;
    case 1:
        {
        b = AdvanceMaterialChanger();
        if (StepScaleTransition() == 0)
            return;
        if (b == 0)
            return;
        ChangeState(2);
        }
        return;
    }
}

void daKuriKing_c::EnterScaleChange() {
    TurnIfPlayerBehind();
    this->mScalePhase = 0;
    this->mSubState = 0;
    unsigned short t = 3 - this->mSizeIndex;
    if (t > 2)
        t = 2;
    this->mTextureSequence.currFrame = t << 12;
    this->mScaleSteps = 3;
}

int daKuriKing_c::UpdateMegaKnockback() {
    if(ApproachAngle(&this->mWalkSpeed, 0, 0xa, 0x100, 1)==0)
        return ChangeState(3);
    return WalkAlongRing();
}

void daKuriKing_c::func_ov074_0211fc34()
{
}

void daKuriKing_c::UpdateKnockback() {
    if (ApproachAngle(&this->mWalkSpeed, 0, 0xa, 0x100, 1) == 0) {
        if (this->mAnmModel.Finished() == 0)
            return;
        if (this->mSizeIndex == 0) {
            daPly_c *player = this->ClosestPlayer();
            if (player->StartTalk(*this, 1) == 0)
                return;
            Message::PrepareTalk();
            if (IsAnimPlaying(1) != 0 || IsAnimPlaying(2) != 0) {
                PlayAnim(0xc);
                return;
            }
            ChangeState(7);
            this->mTalkPlayer = player;
            return;
        }
        ChangeState(6);
        return;
    }
    WalkAlongRing();
}

void daKuriKing_c::EnterKnockback() {
    unsigned char value;

    this->mSizeIndex--;
    value = this->mSizeIndex;
    if (value > 5)
        value = 5;
    this->mSizeIndex = value;
}

void daKuriKing_c::UpdateSummonGoombas() {
    if (IsAnimPlaying(6) != 0) {
        if (FinishTurnAround() == 0)
            return;
        PlayAnim(0xb);
        return;
    }
    if (IsAnimPlaying(0xb) == 0) {
        if (StopWalking() == 0)
            return;
        PlayAnim(0xb);
        return;
    }

    int spawnTarget = data_ov074_02122d80[this->mSizeIndex];
    daPly_c *pl = this->ClosestPlayer();
    if (this->mSizeIndex == 1 && (int)pl->param1 != 3) spawnTarget = spawnTarget + 1;

    if (this->mAnmModel.Finished() == 0)
        return;
    if (DecIfAbove0_Short(&this->mTimer) != 0) return;

    Vector3 pos;
    func_ov074_0212087c((int*)&pos, this, this->mSpawnedCount);
    Vector3 pp;
    pp.x = pos.x;
    pp.y = pos.y;
    pp.z = pos.z;
    this->DisappearPoofDustAt(pp);

    u32 sid = 0xeeee;
    if ((int)pl->param1 != 3) {
        if (this->mSpawnedCount % 2 == 0) sid = 0xeeef;
    }

    daKrb_c *sp2 = (daKrb_c *)dActor_c::Spawn(0xc8, sid, pos, &this->mAngle, this->mAreaId, -1);
    sp2->mTargetUniqueID = this->uniqueID;
    Vector3 v2;
    v2.x = this->mSpawnPosX;
    v2.y = this->mSpawnPosY;
    v2.z = this->mSpawnPosZ;
    func_ov084_021296b0(sp2, &v2);

    sp2->mMinionIndex = this->mSpawnedCount;
    /* Same pool-address increment as mSubState: 0x602 is not an ARM immediate. */
    u8* spawnedCount = (u8*)(((int)&this->mSpawnedCount));
    *spawnedCount = (u8)(*spawnedCount + 1);
    this->mTimer = 2;

    daPly_c *pl2 = this->ClosestPlayer();
    int dv = 3;
    if ((int)pl2->param1 != 3) {
        if (this->mSizeIndex == 1) dv = 5;
    }
    if (this->mSpawnedCount % dv == 0)
        this->mAnmModel.currFrame = 0;

    if (this->mSpawnedCount < spawnTarget) return;
    ChangeState(3);
}

void daKuriKing_c::EnterSummonGoombas() {
    TurnIfPlayerBehind();
    this->mTimer = 0;
}

void daKuriKing_c::func_ov074_0211ffcc() {
    if (IsAnimPlaying(5) == 0) {
        if (StopWalking() == 0)
            return;
        PlayAnim(5);
        return;
    }
  if (this->DistToCPlayer() >= 0x4b0000) {
      ChangeState(3);
  } else {
    if (AngleDiff(this->HorzAngleToCPlayer(), this->mAngleY) < 0x3000) {
        ChangeState(3);
        return;
    }
  }
  if (this->mSpawnedCount != 0) return;
  ChangeState(2);
}

void func_ov074_0212007c(void) {}

void daKuriKing_c::UpdateWalk() {
    if (this->mSpawnedCount == 0) {
        ChangeState(2);
        return;
    }
    if (IsAnimPlaying(0xa) != 0) {
        if (this->mAnmModel.Finished() == 0)
            return;
        PlayAnim(8);
        return;
    }
    ApproachAngle(&this->mWalkSpeed, this->mMaxWalkSpeed, 0xa, 0x20, 1);
    WalkAlongRing();
    if (this->DistToCPlayer() >= 0x4b0000)
        return;
    if (AngleDiff(this->HorzAngleToCPlayer(), this->mAngleY) <= 0x4000)
        return;
    ChangeState(4);
}

void daKuriKing_c::EnterWalk() {
    PlayAnim(0xa);
    UpdateMaxWalkSpeed();
}

void func_02011d20(void);
void daKuriKing_c::UpdateIntroTalkEnd() {
    if (this->mTalkPlayer->GetTalkState() != -1)
        return;
    gCamera->mFlags &= ~8;
    Sound::LoadAndSetMusic_Layer3(0x2d);
    func_02011d20();
    Message::EndTalk();
    ChangeState(2);
}

void func_ov074_021201ec(void) {}

/* daKuriKing_c state-0 tick -- the intro cutscene.
 *
 * Frames the boss for the opening conversation: pins the camera to a fixed
 * look-at/eye pair derived from the boss position, turns the closest player
 * around to face him (angle + 0x7fff), snaps the boss's own facing to
 * mDirection * 0x4500 + mWalkAngle, then runs the talk handshake. daPly_c::StartTalk
 * and daPly_c::ShowMessage each gate the rest of the tick, so a refused talk
 * simply retries next frame; on success the music layer flips once (guarded
 * by mMusicStarted) and the state advances to 1.
 */
void daKuriKing_c::UpdateIntroTalk() {
    struct Ang16 { short v[3]; };
    struct Ang16 ang;
    struct Vector3 look, pos;
    struct Vector3 msgpos[2];
    daPly_c *player;
    dCamera_c* cam;
    short msg;

    player = this->ClosestPlayer();
    cam = gCamera;
    cam->SetFlag_3();

    look.x = this->mPosX;
    look.y = this->mPosY;
    look.z = this->mPosZ;
    pos.x = this->mPosX;
    pos.y = this->mPosY;
    pos.z = this->mPosZ;
    look.x += 0x130000;
    look.y = 0x1cd000;
    look.z -= 0x2a0000;
    pos.x -= 0x2f0000;
    pos.y += 0x20000;
    pos.z += 0x254000;
    cam->SetLookAt(look);
    cam->SetPos(pos);

    ang = *(struct Ang16*)&this->mAngleX;
    ang.v[1] += 0x7fff;
    player->mAngleX = ang.v[0];
    player->mAngleY = ang.v[1];
    player->mAngleZ = ang.v[2];

    {
        int pv = (int)&player->mPosX;
        msgpos[0].x = *(int*)pv;
        msgpos[0].y = *(int*)(pv + 4);
        msgpos[0].z = *(int*)(pv + 8);
    }
    msgpos[1].x = this->mPosX;
    msgpos[1].y = this->mPosY;
    msgpos[1].z = this->mPosZ;
    msgpos[1].y += 0x78000;

    this->mAngleY = this->mDirection * 0x4500 + this->mWalkAngle;
    this->mTalkPlayer = player;
    if (player->StartTalk(*this, 1) == 0) return;
    Message::PrepareTalk();
    if (this->mMusicStarted == 0) {
        Sound::LoadAndSetMusic_Layer3(0x2c);
        this->mMusicStarted = 1;
    }
    msg = (unsigned short)((int)player->param1 + 0xd3);
    if (player->ShowMessage(*this, msg, &msgpos[1], 0, 2) == 0) return;
    func_02012694(0x15a, (const Vector3 *)(&this->mCamSpacePosX));
    ChangeState(1);
}

void func_ov074_021203e0(void) {}

int daKuriKing_c::ChangeState(int i) {
    mState = i;
    int j = mState;
    KuriKingPmf *c = (KuriKingPmf *)this;
    (c->*data_ov074_021230f8.entries[j].pmf[0])();
}

void daKuriKing_c::UpdateState() {
    KuriKingPmf *c = (KuriKingPmf *)this;
    int j = mState;
    (c->*data_ov074_021230f8.entries[j].pmf[1])();
}

int daKuriKing_c::AdvanceMaterialChanger() {
    this->mMaterialChanger.Advance();
    unsigned char idx = this->mSizeIndex;
    unsigned int val = this->mMaterialChanger.currFrame;
    unsigned short tbl = data_ov074_02122e04[idx];
    unsigned int v = (val << 4) >> 0x10;
    if (v >= tbl) {
        this->mMaterialChanger.currFrame = (unsigned int)tbl << 0xc;
        return 1;
    }
    return 0;
}

/* daKuriKing_c shrink/grow interpolation tick.
 *
 * Called from UpdateDefeat and UpdateScaleChange. data_ov074_02122e4c holds
 * the boss's four sizes as floats (3.0, 2.333, 1.666 and 1.0 in Fix12 units);
 * mTargetScale is the next size plus the gap back to the current one,
 * divided by the steps left in mScaleSteps. Math_Function_0203b14c eases
 * mCurrentScale toward that target; unless the last step has just finished,
 * the scale triple then gets a wobble from the sine and cosine of
 * mScalePhase, which advances 0x1230 per frame. In state 6, each finished
 * step except the last also spits particle 0xb1 at 400.0 * mCurrentScale
 * above the boss.
 */
int daKuriKing_c::StepScaleTransition() {
    struct Vector3 v;
    int d, k;

    mTargetScale = data_ov074_02122e4c[mSizeIndex + 1] +
                   FX_Div(data_ov074_02122e4c[mSizeIndex] - data_ov074_02122e4c[mSizeIndex + 1],
                          mScaleSteps << 12);

    if (Math_Function_0203b14c(&this->mCurrentScale, this->mTargetScale, 0x78, 0x100, 0x20) == 0) {
        if (this->mScalePhase == 0) {
            this->mScaleX = this->mCurrentScale;
            this->mScaleY = this->mCurrentScale;
            this->mScaleZ = this->mCurrentScale;
        }
        if (this->mScaleSteps <= 1) return 1;
        this->mScaleSteps--;
        if (this->mState == 6) {
            v.x = this->mPosX;
            v.y = this->mPosY;
            v.z = this->mPosZ;
            v.y += FX_MUL(this->mCurrentScale, 0x190000);
            Particle::System::NewSimple(kPtclHugeLandingDust, ((int *)&v)[0], ((int *)&v)[1],
                                        ((int *)&v)[2]);
        }
    }
    k = ((u16)mScalePhase >> 4) * 2;
    d = mTargetScale - mCurrentScale + 0x11e;
    mScaleY = mCurrentScale + FX_MUL(d, FX_SinCosTable_[k]);
    mScaleX = mCurrentScale;
    mScaleX = mCurrentScale + FX_MUL(d, FX_SinCosTable_[k + 1]);
    this->mScaleZ = this->mScaleX;
    this->mScalePhase += 0x1230;
    return 0;
}

int daKuriKing_c::StopWalking() {
    if (IsAnimPlaying(8) != 0) {
        if (ApproachAngle(&this->mWalkSpeed, 0, 0x1e, 0x20, 1) == 0) {
            if ((unsigned short)(this->mAnmModel.currFrame >> 12) == 0) {
                PlayAnim(9);
            }
        } else {
            WalkAlongRing();
        }
    } else if (IsAnimPlaying(0xa) != 0) {
        PlayAnim(9);
    } else if (IsAnimPlaying(9) != 0) {
        if (this->mAnmModel.Finished() != 0)
            return 1;
    } else {
        return 1;
    }
    return 0;
}

/* If anim 6 is current and mAnmModel has finished, flip mAngleY by half
 * a turn and return 1. Otherwise return 0.
 */
int daKuriKing_c::FinishTurnAround() {
    if (IsAnimPlaying(6)) {
        if (this->mAnmModel.Finished()) {
            this->mAngleY += 0x8000;
            return 1;
        }
    }
    return 0;
}

void daKuriKing_c::TurnIfPlayerBehind() {
    int ang = (short)(this->mWalkAngle + this->mDirection * 0x4500);
    if (AngleDiff(ang, this->HorzAngleToCPlayer()) <= 0x4000)
        return;
    PlayAnim(6);
    this->mDirection *= -1;
}

void func_ov074_0212087c(int *out, daKuriKing_c *self, int n_) {
    int n;

    n = n_;

    out[0] = self->mPosX;
    out[1] = self->mPosY;
    out[2] = self->mPosZ;

    if ((int)self->ClosestPlayer()->param1 == 3) {
        int rem;
        int quo;
        int scale;
        int q;
        int sum;
        int prod;
        int diff;
        u8 mode;

        rem = n % 3;
        quo = n / 3;
        mode = self->mSizeIndex;
        if (mode == 3) {
            scale = 0x546000;
        } else if (mode == 2) {
            scale = quo * 0xb4000 + 0x4ec000;
        } else {
            scale = quo * 0xb4000 + 0x438000;
        }

        q = self->mScaleX / 0x1000;
        sum = (s16)(q * 0x350 + 0x500) + (s16)(rem * 0x600);
        prod = self->mDirection * sum;
        diff = (self->mWalkAngle + self->mMaxWalkSpeed) - prod;
        out[0] = FX_MUL(scale, FX_SinIdx((u16)(s16)diff >> 4));
        out[1] += 0x32000;
        out[2] = FX_MUL(scale, FX_CosIdx((u16)(s16)diff >> 4));
    } else {
        int lim;
        int one;
        int sign;
        int rem2;
        int scale2;
        int q2;
        int sum2;
        int prod2;
        int prod3;
        int diff2;

        lim = 3;
        if (self->mSizeIndex == 1)
            lim = 5;

        if (n < lim) {
            one = 1;
            sign = -1;
            rem2 = n % lim;
            scale2 = 0x4e2000 + (rem2 * 0x64 << 12);
        } else {
            one = 1;
            sign = one;
            rem2 = n % lim;
            scale2 = 0x4e2000 + (rem2 * 0x64 << 12);
        }

        q2 = self->mScaleX / 0x1000;
        sum2 = (s16)(q2 * 0x350 + 0x250) + (s16)(one * 0x600);
        prod2 = self->mDirection * sum2;
        prod3 = sign * prod2;
        diff2 = (self->mWalkAngle + self->mWalkSpeed) - prod3;
        out[0] = FX_MUL(scale2, FX_SinIdx((u16)(s16)diff2 >> 4));
        out[1] += 0x32000;
        out[2] = FX_MUL(scale2, FX_CosIdx((u16)(s16)diff2 >> 4));
    }
}

int daKuriKing_c::CalcWalkAnimSpeed() {
    int num = this->mWalkSpeed << 12;
    int den;
    int r;
    if (num < 0) num = -num;
    den = data_ov074_0212291c[this->mSizeIndex] + (data_ov074_02122dfc[3] << 12) + 1;
    r = FX_Div(num, den);
    if (r < 0x1000) return 0x1000;
    if (r > 0x3000) return 0x3000;
    return r;
}

void daKuriKing_c::UpdateMaxWalkSpeed() {
    int idx = this->mSizeIndex;
    int base = this->mDirection;
    unsigned short scale = data_ov074_02122dfc[idx];
    this->mMaxWalkSpeed = base * scale;
}

void daKuriKing_c::SpawnHitParticles(dActor_c *other, dCcAcPos_c *clsn, int mode) {
    Vector3 v, dst, src, delta;
    s32 aX, aY;
    s32 iX, iY;
    s32 s, w;

    dst.x = this->mPosX;
    dst.y = this->mPosY;
    dst.z = this->mPosZ;
    {
        Vector3 *pv = &other->mPos;
        src.x = pv->x;
        src.y = pv->y;
        src.z = pv->z;
    }
    {
        s32 dy = clsn->height;
        w = clsn->radius;
        dst.y += dy;
    }
    src.y += 0x46000;
    Vec3_Sub(&delta, &src, &dst);

    v.x = delta.x;
    v.y = delta.y;
    v.z = delta.z;
    aX = cstd::atan2(v.x, v.z);
    aY = cstd::atan2(v.y, Vec3_HorzLen(&v));

    s = FX_MUL(w, FX_CosIdx((u16)aY >> 4));
    dst.x += FX_MUL(s, FX_SinIdx((u16)aX >> 4));
    dst.y += FX_MUL(w, FX_SinIdx((u16)aY >> 4));
    dst.z += FX_MUL(s, FX_CosIdx((u16)aX >> 4));

    switch (mode) {
    case 0:
        Particle::System::NewSimple(kPtclEnemyHit, dst.x, dst.y, dst.z);
        break;
    case 1:
        Particle::System::NewSimple(kPtclKnockAwayA, dst.x, dst.y, dst.z);
        Particle::System::NewSimple(kPtclKnockAwayB, dst.x, dst.y, dst.z);
        break;
    }
}

void daKuriKing_c::HandleCylinderHits() {
    struct Vector3 v0;
    struct Vector3 v1;
    struct Vector3 v2;
    struct Vector3 v3;
    int i;
    dActor_c *player;
    dCcAcPos_c *cur;
    dCcAcPos_c *p;
    int hit;
    int dir;
    int dir2;
    u32 s;
    u32 flags;
    u16 type;
    int b;
    int b2;

    DecIfAbove0_Byte(&this->mHurtTimer);
    s = this->mState;
    if (s == 1)
        return;
    if (s == 2)
        return;
    if (s - 5 <= 3)
        return;

    player = 0;
    i = 0;
    p = this->mdCc_cs;
    do {
        cur = p;
        if (cur->otherOwner != 0)
            player = dActor_c::FindWithID(cur->otherOwner);
        if (player != 0)
            break;
        i++;
        p += 1;
    } while (i < 4);
    if (player == 0)
        return;

    flags = cur->hitFlags;
    hit = 0;
    if (flags & 0x2000) {
        hit = 1;
    } else {
        type = player->actorID;
        b = (int)(type == 0xbf);
        if (b != 0) {
            daPly_c *pl = (daPly_c *)player;
            if (this->mHurtTimer == 0) {
                if (!(flags & 0x26fe0) && pl->mIsTakingDamage == 0) {
                    if (cur == &this->mdCc_cs[2]) {
                        if (this->mWalkSpeed != 0) {
                            pl->Unk_020c6a10(2);
                            this->mHurtTimer = 0x1e;
                        } else {
                            v1.x = this->mPosX;
                            v1.y = this->mPosY;
                            v1.z = this->mPosZ;
                            pl->Hurt(v1, 1, 0x5000, 1, 0, 1);
                        }
                    } else if (cur == &this->mdCc_cs[3]) {
                        if (this->mWalkSpeed != 0) {
                            pl->Unk_020c6a10(2);
                            this->mHurtTimer = 0x1e;
                        } else {
                            v2.x = this->mPosX;
                            v2.y = this->mPosY;
                            v2.z = this->mPosZ;
                            pl->Hurt(v2, 1, 0x5000, 1, 0, 1);
                        }
                    } else {
                        v3.x = this->mPosX;
                        v3.y = this->mPosY;
                        v3.z = this->mPosZ;
                        pl->Hurt(v3, 1, 0x5000, 1, 0, 1);
                        this->mHurtTimer = 0x10;
                    }
                }
                if (pl->mIsMega != 0) {
                    if (cur->hitFlags & 0x3c0) {
                        hit = 1;
                    } else {
                        int ang = Vec3_HorzAngle(&this->mPos, &pl->mPos);
                        dir = 1;
                        if (AngleDiff(ang, this->mAngleY) > 0x4000)
                            dir = -1;
                        PlayAnim(7);
                        ChangeState(8);
                        SpawnHitParticles(pl, cur, 0);
                        func_02012694(0x15a, (const Vector3 *)(&this->mCamSpacePosX));
                        this->mWalkSpeed = dir * (this->mDirection * -0xc8);
                    }
                } else {
                    if (cur == &this->mdCc_cs[1]) {
                        if (cur->hitFlags & 0x80) {
                            if (this->mState == 4)
                                ChangeState(2);
                            else
                                ChangeState(4);
                            if ((int)pl->param1 == 0) {
                                v0.x = this->mPosX;
                                v0.y = this->mPosY;
                                v0.z = this->mPosZ;
                                if (this->mMegaMushroomID == 0 &&
                                    dActor_c::FindWithID(this->mMegaMushroomID) == 0) {
                                    int k =
                                        ((u16)(s16)(this->mDirection *
                                                        ((s16)((this->mScaleX / 0x1000) * 0x350 +
                                                               0x500) +
                                                         0x600) +
                                                    (this->mWalkAngle + this->mMaxWalkSpeed))) >>
                                        4;
                                    v0.x = FX_MUL(FX_SinIdx(k), 0x546000LL);
                                    v0.y += 0x32000;
                                    v0.z = FX_MUL(FX_CosIdx(k), 0x546000LL);
                                    this->mMegaMushroomID =
                                        dActor_c::Spawn(0x115, 0, v0, 0, this->mAreaId, -1)
                                            ->uniqueID;
                                }
                            }
                        }
                    }
                }
            }
        } else {
            b2 = (int)(type == 0xc8);
            if (b2 != 0) {
                if (flags & 0x22000)
                    hit = 1;
            }
        }
    }
    if (hit == 0)
        return;
    func_02012694(0x15b, (const Vector3 *)(&this->mCamSpacePosX));
    {
        int ang2 = Vec3_HorzAngle(&this->mPos, &player->mPos);
        dir2 = 1;
        if (AngleDiff(ang2, this->mAngleY) > 0x4000) {
            PlayAnim(dir2);
            dir2 = -1;
        } else {
            PlayAnim(2);
        }
    }
    this->mWalkSpeed = dir2 * (this->mDirection * -0x190);
    ChangeState(5);
    SpawnHitParticles(player, cur, 1);
}

/* World position of model bone `bone`: that bone's transform composed with
 * mBodyMatrix, translation row out, shifted back up from model
 * space (<< 3). */
void func_ov074_02121270(Vector3 *out, daKuriKing_c *self, int bone) {
    Vector3 *v = out;
    Matrix4x3 *boneMat;
    v->x = 0;
    v->y = 0;
    v->z = 0;
    data_020a0e68 = *(struct Matrix4x3 *)self->mBodyMatrix;
    boneMat = self->mAnmModel.data.transforms + bone;
    MTX_Concat43(boneMat, &data_020a0e68, &data_020a0e68);
    v->x = data_020a0e68.m[9];
    v->y = data_020a0e68.m[10];
    v->z = data_020a0e68.m[11];
    Vec3_LslInPlace(v, 3);
}

extern "C" struct Matrix4x3 data_020a0e68;
void daKuriKing_c::UpdateBodyMatrix() {
    struct Vector3 v;
    Vec3_Asr(&v, &this->mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, this->mAngleY);
    Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, this->mScaleX, this->mScaleY, this->mScaleZ);
    *(struct Matrix4x3 *)this->mBodyMatrix = data_020a0e68;
}

/* Sizes the four collision cylinders from the current scale and places the
 * three foot cylinders under the model's bone positions, then works out which
 * feet are touching the ground this frame: a foot counts as stepped-on only
 * on the frame its ground test first reads below the body. */
struct KuriKingBoneIdx { int v[3]; };

void daKuriKing_c::UpdateCylindersAndFootsteps() {
    KuriKingBoneIdx bone = *(const KuriKingBoneIdx *)data_ov074_02122e24;
    int yoff[3], rad[3], hgt[3];
    Vector3 v[3];
    int i;
    int h0, r0, y0;
    int d, a, b;
    int tx, ty, tz;

    y0 = FX_MUL(this->mScaleY, 0x40000);
    rad[1] = FX_MUL(this->mScaleX, 0x50000);
    hgt[1] = FX_MUL(this->mScaleY, 0x40000);
    yoff[1] = FX_MUL(this->mScaleY, -0x10000);
    rad[2] = FX_MUL(this->mScaleX, 0x50000);
    hgt[2] = FX_MUL(this->mScaleY, 0x40000);
    yoff[2] = FX_MUL(this->mScaleY, -0x10000);
    h0 = FX_MUL(this->mScaleY, 0x96000);
    r0 = FX_MUL(this->mScaleX, 0x5a000);
    yoff[0] = FX_MUL(this->mScaleY, 0x14000);
    rad[0] = FX_MUL(this->mScaleX, 0xb4000);
    hgt[0] = FX_MUL(this->mScaleY, 0x104000);
    for (i = 0; i < 3; i++) {
        int ax, ay, az;
        func_ov074_02121270(&v[2], this, bone.v[i]);
        this->mCylClsnPos[i].x = v[2].x;
        this->mCylClsnPos[i].y = v[2].y;
        this->mCylClsnPos[i].z = v[2].z;
        ax = this->mCylClsnPos[i].x;
        az = this->mCylClsnPos[i].z;
        ay = this->mCylClsnPos[i].y + yoff[i];
        this->mdCc_cs[i + 1].radius = rad[i];
        this->mdCc_cs[i + 1].height = hgt[i];
        this->mdCc_cs[i + 1].pos.x = ax;
        this->mdCc_cs[i + 1].pos.y = ay;
        this->mdCc_cs[i + 1].pos.z = az;
    }

    tx = this->mPosX;
    ty = this->mPosY + y0;
    tz = this->mPosZ;
    this->mdCc_cs[0].radius = r0;
    this->mdCc_cs[0].height = h0;
    this->mdCc_cs[0].pos.x = tx;
    this->mdCc_cs[0].pos.y = ty;
    this->mdCc_cs[0].pos.z = tz;

    d = FX_MUL(this->mScaleY, -0x4a000);
    a = this->mCylClsnPos[2].y + d;
    b = this->mCylClsnPos[1].y + d;
    if (a < this->mPosY) {
        if (this->mFoot2Latch == 0) {
            this->mLeftFootSteppedOnGround = 1;
        } else {
            this->mLeftFootSteppedOnGround = 0;
        }
        this->mFoot2Latch = 1;
    } else {
        this->mFoot2Latch = 0;
        this->mLeftFootSteppedOnGround = 0;
    }
    if (b < this->mPosY) {
        if (this->mFoot1Latch == 0) {
            this->mRightFootSteppedOnGround = 1;
        } else {
            this->mRightFootSteppedOnGround = 0;
        }
        this->mFoot1Latch = 1;
    } else {
        this->mFoot1Latch = 0;
        this->mRightFootSteppedOnGround = 0;
    }
    if (this->mFoot1Latch == 0) {
        this->mdCc_cs[2].flags |= 0x20;
    } else {
        this->mdCc_cs[2].flags &= ~0x20;
    }
    if (this->mFoot2Latch == 0) {
        this->mdCc_cs[3].flags |= 0x20;
    } else {
        this->mdCc_cs[3].flags &= ~0x20;
    }
}

/* Advances mWalkAngle by mWalkSpeed while on the ground and re-derives the
 * walk velocity from the sine table, then eases mAngleY toward the
 * direction-scaled heading. */
/* Returns int so callers can `return WalkAlongRing(c)`; the body still
 * falls off the end, which is what the ROM's code does. */
int daKuriKing_c::WalkAlongRing() {
    if (this->mWithMeshClsn.IsOnGround()) {
        this->mWalkAngle += this->mWalkSpeed;
        this->mPosX = FX_MUL(FX_SinIdx(((u16)this->mWalkAngle) >> 4), 0x546000);
        this->mPosZ = FX_MUL(FX_CosIdx(((u16)this->mWalkAngle) >> 4), 0x546000);
    }
    ApproachAngle(&this->mAngleY, (this->mDirection * 0x4500) + this->mWalkAngle, 0xa, 0x800, 0x10);
}

void daKuriKing_c::UpdateShadows() {
    Matrix4x3 mtx;
    int i, height, rad, footX, posY, radius; /* footX is never used: it keeps the @NNN count */
    char *self, *src, *m;
    dExtShadowModel_c *shadow;

    if (this->mNoShadow != 0)
        return;

    /* self and src are this shifted by i shadow matrices (0x30) and i
     * mCylClsnPos entries (0xc), read back through the class so each load
     * keeps the member offset: typed pointers or indexing re-base them and
     * the loop changes. Words 9..11 are a matrix's translation row. */
    mtx = IDENTITY_MATRIX4X3;
    self = (char *)this;
    src = (char *)this;
    m = (char *)mShadowMatrices;
    shadow = mShadowModels;
    for (i = 0; i < 3; i++) {
        height = this->mPosY - this->mSpawnPosY;
        if (height <= 0x1000)
            height = 0x1000;
        rad = (this->mdCc_cs[i + 1].radius << 1) - FX_MUL(height, 0x180);
        if (rad < 0xa000)
            rad = 0xa000;
        radius = rad;
        *(Matrix4x3 *)((daKuriKing_c *)self)->mShadowMatrices[0] = mtx;
        ((daKuriKing_c *)self)->mShadowMatrices[0][9] =
            ((daKuriKing_c *)src)->mCylClsnPos[0].x >> 3;
        posY = this->mPosY;
        /* Both arms are the same; the select is never needed for codegen,
           but its compiler temporaries keep the @2113.. pins (dropping it
           renumbers by 4). */
        ((daKuriKing_c *)self)->mShadowMatrices[0][10] = (radius ? posY : posY) >> 3;
        height += 0x28000;
        ((daKuriKing_c *)self)->mShadowMatrices[0][11] =
            ((daKuriKing_c *)src)->mCylClsnPos[0].z >> 3;
        DropShadowRadHeight(*shadow, *(Matrix4x3 *)m, radius, height, 0xf);
        self += 0x30;
        src += 0xc;
        m += 0x30;
        shadow++;
    }
}

/* Fills the model matrix from mAngleY and the position (model space is
 * world >> 3). */
void daKuriKing_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;
}

void daKuriKing_c::UpdateAnimation() {
    if (IsAnimPlaying(8)) {
        mAnmModel.speed = CalcWalkAnimSpeed();
    } else if (IsAnimPlaying(0xb)) {
        if ((((unsigned int)mAnmModel.currFrame << 4) >> 0x10) == 8) {
            func_02012694(0x15d, (const Vector3 *)(&mCamSpacePosX));
        }
    } else {
        mAnmModel.speed = 0x1000;
    }
    mAnmModel.Advance();
    mTextureTransformer.Advance();
}

/* The animation table behind data_ov074_02122f34: one record per clip --
 * the SharedFilePtr that holds the BCA_File, the matching BTA_File for the
 * texture transformer, and SetAnim's animation index. */
struct KuriKingAnimFile { u16 fileID; u8 numRefs; void *filePtr; };
struct KuriKingAnimEntry { KuriKingAnimFile *anim; void *btaFile; int animArg; };
extern KuriKingAnimEntry data_ov074_02122f34[];

/* Is clip `idx` the one currently playing: compare its file pointer with the
 * dExtAnmModel_c's. */
int daKuriKing_c::IsAnimPlaying(int idx) {
    KuriKingAnimFile *anim = data_ov074_02122f34[idx].anim;
    return this->mAnmModel.file == anim->filePtr;
}

/* Start clip `idx`: its BCA on the dExtAnmModel_c and its BTA on the texture
 * transformer. */
void daKuriKing_c::PlayAnim(int idx) {
    int off = idx * 0xc;
    void* file1 = data_ov074_02122f34[idx].anim->filePtr;
    int arg2 = *(int*)(data_ov074_02122f3c + off);
    this->mAnmModel.SetAnim((BCA_File *)file1, arg2, 0x1000, 0);
    this->mTextureTransformer.SetFile(**(BTA_File **)(data_ov074_02122f38 + off), 0, 0x1000, 0);
}

int daKuriKing_c::CleanupResources()
{
    int i;

    if (param1 == 0x1111) {
        return CleanupVanishResources();
    }
    data_ov002_0210da30.Release();
    UnloadKeyModels(2);
    data_ov084_02130cf8.Release();
    for (i = 0; i < 7; i++)
        sKurikingMinionAnimationFiles[i]->Release();
    gKurikingModelModelFile.Release();
    for (i = 0; i < 0xc; i++)
        sKurikingAnimationFiles[i]->Release();
    gKurikingAngerTextureSequenceFile.Release();
    return 1;
}

int daKuriKing_c::Render()
{
    if (param1 == 0x1111)
        return RenderVanish();
    if (mShouldRender == 0) return 1;
    mAnmModel.Render(&mScale);
    mTextureSequence.Update(mAnmModel.data);
    mMaterialChanger.Update(mAnmModel.data);
    mTextureTransformer.Update(mAnmModel.data);
    return 1;
}

int daKuriKing_c::Behavior()
{
    int t;
    int u;
    int quake;

    if (param1 == 0x1111) {
        return BehaviorVanish();
    }

    t = FX_MUL(mWalkSpeed, 0x646);
    u = FX_MUL(t, 0x546000);
    mGoombaTargetSpeed = u < 0 ? -u : u;

    /* The camera frames the main boss (not param1 0x1111) every frame
       except in state 7 (defeat). */
    if (mState != 7) {
        gCamera->mFramedActor = this;
    }

    UpdateAnimation();
    UpdateState();
    UpdatePos(0);
    UpdateWMClsn(mWithMeshClsn, 0);
    HandleCylinderHits();
    UpdateCylindersAndFootsteps();

    {
        int i;
        dCcAcPos_c *p = mdCc_cs;
        for (i = 0; i < 4; i++) {
            p->Clear();
            p->Update();
            p += 1;
        }
    }

    if (mState != 1 && mState != 0) {
        quake = data_ov074_0212290c[mSizeIndex];

        if (mLeftFootSteppedOnGround != 0) {
            if (mSizeIndex >= 2) {
                Vector3 v;
                v.x = mCylClsnPos[2].x;
                v.y = mCylClsnPos[2].y;
                v.z = mCylClsnPos[2].z;
                HugeLandingDustAt(v, 1);
            } else {
                Vector3 v;
                v.x = mCylClsnPos[2].x;
                v.y = mCylClsnPos[2].y;
                v.z = mCylClsnPos[2].z;
                LandingDustAt(v, 1);
            }
            func_02012694(0x15e, (const Vector3 *)(&mCamSpacePosX));
            {
                Vector3 v;
                v.x = mCylClsnPos[2].x;
                v.y = mCylClsnPos[2].y;
                v.z = mCylClsnPos[2].z;
                Earthquake(v, quake);
            }
        }

        if (mRightFootSteppedOnGround != 0) {
            if (mSizeIndex >= 2) {
                Vector3 v;
                v.x = mCylClsnPos[1].x;
                v.y = mCylClsnPos[1].y;
                v.z = mCylClsnPos[1].z;
                HugeLandingDustAt(v, 1);
            } else {
                Vector3 v;
                v.x = mCylClsnPos[1].x;
                v.y = mCylClsnPos[1].y;
                v.z = mCylClsnPos[1].z;
                LandingDustAt(v, 1);
            }
            func_02012694(0x15e, (const Vector3 *)(&mCamSpacePosX));
            {
                Vector3 v;
                v.x = mCylClsnPos[1].x;
                v.y = mCylClsnPos[1].y;
                v.z = mCylClsnPos[1].z;
                Earthquake(v, quake);
            }
        }
    }

    UpdateBodyMatrix();
    UpdateModelMatrix();
    UpdateShadows();
    return 1;
}

int daKuriKing_c::InitResources()
{
    s32 i;
    s32 j;
    s32 k;
    void *p;
    Vector3 pos;

    if (param1 == 0x1111) {
        return InitVanishResources();
    }

    dExtSimpleModel_c::LoadFile(data_ov002_0210da30);
    LoadKeyModels(2);
    dExtSimpleModel_c::LoadFile(data_ov084_02130cf8);

    i = 0;
    do {
        dExtFrameCtrl_c::LoadFile(*sKurikingMinionAnimationFiles[i]);
        i++;
    } while (i < 7);

    i = 0;
    do {
        dExtFrameCtrl_c::LoadFile(*sKurikingAnimationFiles[i]);
        i++;
    } while (i < 0xc);

    dExtAnmTexPat_c::LoadFile(gKurikingAngerTextureSequenceFile);
    p = dExtSimpleModel_c::LoadFile(gKurikingModelModelFile);
    mAnmModel.SetFile((BMD_File *)p, 1, -1);

    j = 0;
    do {
        mShadowModels[j].InitCylinder();
        j++;
    } while (j < 3);

    mSizeIndex = 3;
    p = (void *)(s32)data_ov074_02122e4c[mSizeIndex];
    mScaleX = (s32)p;
    mScaleY = (s32)p;
    mScaleZ = (s32)p;
    mVertAccel = -0xa000;
    mTerminalVelocity = -0x3c000;

    mSpawnPosX = mPosX;
    mSpawnPosY = mPosY;
    mSpawnPosZ = mPosZ;

    k = 0;
    do {
        mdCc_cs[k].Init(this, mPos, mScaleX * 0xa, mScaleY * 0xa, 0x200004, 0x26fe0);
        k++;
    } while (k < 4);

    mAnmModel.SetAnim((BCA_File *)data_ov074_02123030.filePtr, 0, 0x1000, 0);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)gKurikingModelModelFile.filePtr,
                             *(BTP_File *)gKurikingAngerTextureSequenceFile.filePtr);
    dExtAnmMaterial_c::Prepare(*(BMD_File *)gKurikingModelModelFile.filePtr, data_ov010_021123f4);

    i = 0;
    do {
        dExtAnmTexSRT_c::Prepare(*(BMD_File *)gKurikingModelModelFile.filePtr,
                                 *(BTA_File *)data_ov074_02122e5c[i]);
        i++;
    } while (i < 7);

    mTextureSequence.SetFile(*(BTP_File *)gKurikingAngerTextureSequenceFile.filePtr, 0, 0x1000, 0);
    mMaterialChanger.SetFile(data_ov010_021123f4, 0x40000000, 0x1000, 0);
    mTextureTransformer.SetFile(data_ov010_021124ac, 0, 0x1000, 0);

    mTextureSequence.speed = 0;
    mMaterialChanger.currFrame = (s32)(data_ov074_02122e04[mSizeIndex] << 0xc);

    mWithMeshClsn.Init(this, 0x14000, 0x14000, 0, 0);

    mDirection = -1;
    mMaxWalkSpeed = (s16)(mDirection * data_ov074_02122dfc[mSizeIndex]);
    mWalkSpeed = 0;
    mWalkAngle = 0x1000;
    mWalkAngle += mWalkSpeed;

    mPosX = FX_MUL(FX_SinIdx((u16)mWalkAngle >> 4), 0x546000);
    mPosZ = FX_MUL(FX_CosIdx((u16)mWalkAngle >> 4), 0x546000);

    {
        s32 a2;
        s32 a1;
        s32 a0;
        s32 a3;
        a2 = mPosY;
        a1 = mPosZ;
        a0 = mPosX;
        a3 = a2 + 0x64000;
        pos.x = a0;
        pos.y = a3;
        pos.z = a1;
    }

    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn() != 0) {
        mPosY = ground.clsnY;
    }

    UpdateBodyMatrix();
    UpdateModelMatrix();

    mMegaMushroomID = 0;
    mShouldRender = 1;
    mCurrentScale = 0x1000;

    return 1;
}

/* The vanish variant's file teardown, from CleanupResources. */
int daKuriKing_c::CleanupVanishResources() {
    data_ov084_02130cf8.Release();
    int i = 0;
    do {
        sKurikingMinionAnimationFiles[i]->Release();
        i++;
    } while (i < 7);
    return 1;
}

/* The vanish variant's render: build the model matrix out of the shared
 * scratch matrix -- position, then +0x6400 up, rotate by the Euler angles,
 * then back down -- and draw the model if visible. */
int daKuriKing_c::RenderVanish() {
    Vector3 v;
    Vec3_Asr(&v, &this->mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, 0x6400, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68, this->mAngleX, this->mAngleY, this->mAngleZ);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, -0x6400, 0);
    this->mAnmModel.mat4x3 = data_020a0e68;

    if (this->mShouldRender == 0) return 1;

    this->mAnmModel.Render(0);
    this->mMaterialChanger.Update(this->mAnmModel.data);
    return 1;
}

/* The vanish variant's tick: once mLaunched the clone is a thrown projectile --
 * it bounces on the ground (0x13a/0x13b), dies on water/toxic floor or when
 * mTimer runs out, and tumbles while airborne. */
int daKuriKing_c::BehaviorVanish() {
    Vector3 pos;

    if (this->mLaunched == 0) {
        return 1;
    }

    if (DecIfAbove0_Short(&this->mTimer) == 0 || this->mSubState >= 3) {
        int b = ((this->mFlags & 8) != 0) ? 1 : 0;
        if (b == 0) {
            this->PoofDust();
        }
        this->MarkForDestruction();
        return 1;
    }

    if (this->mWithMeshClsn.JustHitGround()) {
        int b = ((this->mFlags & 8) != 0) ? 1 : 0;
        if (b == 0) {
            if (this->mSubState == 0) {
                func_02012694(0x13a, (const Vector3 *)(&this->mCamSpacePosX));
            } else {
                func_02012694(0x13b, (const Vector3 *)(&this->mCamSpacePosX));
            }
        }
        this->mSubState++;

        this->mVertSpeed = this->mHorzSpeed + 0xa000;
    }

    {
        int yy = this->mPosY;
        int zz = this->mPosZ;
        int xx = this->mPosX;
        int py = yy + 0x190000;
        pos.x = xx;
        pos.y = py;
        pos.z = zz;
    }

    dBgCh_Gnd rg;
    rg.StartDetectingWater();
    rg.StartDetectingToxic();
    rg.StopDetectingOrdinary();
    rg.SetObjAndPos(pos, this);

    if (rg.DetectClsn() != 0
        && func_02037e20((int *)&rg.surface) != 0
        && rg.clsnY != (int)0x80000000
        && this->mPosY < rg.clsnY) {
        this->PoofDust();
        this->MarkForDestruction();
        return 1;
    }

    if (this->mTimer < 0x60) {
        this->UpdatePos(0);
        this->UpdateWMClsn(this->mWithMeshClsn, 0);

        this->mAngleX += 0x1800;
    }

    /* One drop shadow, through the first shadow matrix. */
    *(Matrix4x3 *)mShadowMatrices[0] = IDENTITY_MATRIX4X3;

    mShadowMatrices[0][9] = this->mPosX >> 3;
    mShadowMatrices[0][10] = this->mPosY >> 3;
    mShadowMatrices[0][11] = this->mPosZ >> 3;

    DropShadowRadHeight(this->mShadowModels[0], *(Matrix4x3 *)mShadowMatrices[0], 0x8c000, 0x3e8000,
                        0xf);

    return 1;
}

/* The vanish variant's InitResources: loads the seven shared animation files,
 * binds the model, a single shadow cylinder, material and texture animators,
 * then seeds the rest angles and the actor-cylinder Init. */
int daKuriKing_c::InitVanishResources() {
    int i;

    dExtSimpleModel_c::LoadFile(data_ov084_02130cf8);
    for (i = 0; i < 7; i++)
        dExtFrameCtrl_c::LoadFile(*sKurikingMinionAnimationFiles[i]);

    if (this->mAnmModel.SetFile((BMD_File *)data_ov084_02130cf8.filePtr, 1, 1) == 0)
        return 0;

    if (this->mShadowModels[0].InitCylinder() == 0)
        return 0;

    dExtAnmMaterial_c::Prepare(*(BMD_File *)data_ov084_02130cf8.filePtr,
                               *(BMA_File *)&data_ov084_0213089c);

    this->mMaterialChanger.SetFile(*(BMA_File *)&data_ov084_0213089c, 0x40000000, 0x1000, 0);

    this->mAnmModel.SetAnim((BCA_File *)data_ov084_02130cc8.filePtr, 0, 0x1000, 0);

    this->mPrevAngleX = this->mAngleX;
    this->mPrevAngleY = this->mAngleY;
    this->mPrevAngleZ = this->mAngleZ;
    this->mTerminalVelocity = -0x3c000;
    this->mMegaMushroomID = 0;

    this->mWithMeshClsn.Init(this, 0x14000, 0x14000, (Vector3_16 *)0, 0);

    return 1;
}

/* Static-resource ownership (was the handwritten __sinit_ov074_02122978 shard,
 * 0x3f8). Definition order is the retail initializer's construction order:
 * the model handle (file 0x395), the texture-sequence handle (file 0x390),
 * then the twelve animation handles. Their registration nodes are compiler
 * temporaries, like the PMF descriptors the table initializer below
 * materializes. */
KuriKingModelFilePtr gKurikingModelModelFile(ASSET_KURIKING_MODEL_BMD);
KuriKingTexSeqFilePtr gKurikingAngerTextureSequenceFile(ASSET_KURIKING_ANGER_BTP);
KuriKingAnimationFilePtr gKurikingAngerAnimationFile(ASSET_KURIKING_ANGER_BCA);
KuriKingAnimationFilePtr gKurikingDamageBAnimationFile(ASSET_KURIKING_DAMAGE_B_BCA);
KuriKingAnimationFilePtr gKurikingDamageFAnimationFile(ASSET_KURIKING_DAMAGE_F_BCA);
KuriKingAnimationFilePtr gKurikingDownStartAnimationFile(ASSET_KURIKING_DOWN_START_BCA);
KuriKingAnimationFilePtr gKurikingDownAnimationFile(ASSET_KURIKING_DOWN_BCA);
KuriKingAnimationFilePtr gKurikingSerchWaitAnimationFile(ASSET_KURIKING_SERCH_WAIT_BCA);
KuriKingAnimationFilePtr gKurikingTurnAnimationFile(ASSET_KURIKING_TURN_BCA);
KuriKingAnimationFilePtr data_ov074_02123030(ASSET_KURIKING_WAIT_BCA);
KuriKingAnimationFilePtr gKurikingWalkAnimationFile(ASSET_KURIKING_WALK_BCA);
KuriKingAnimationFilePtr gKurikingWalkEndAnimationFile(ASSET_KURIKING_WALK_END_BCA);
KuriKingAnimationFilePtr gKurikingWalkStartAnimationFile(ASSET_KURIKING_WALK_START_BCA);
KuriKingAnimationFilePtr gKurikingSpawnAnimationFile(ASSET_KURIKING_SPAWN_BCA);

/* The three descriptors that target free stubs, not members, stay named
 * here in ROM address order. The fifteen member descriptors are named
 * inline in the table initializer above; the compiler materializes those
 * as temporaries (a same-TU member PMF is not a static initializer). */
KuriKingFreeState data_ov074_02122d9c = { (void (*)())func_ov074_0212007c };
KuriKingFreeState data_ov074_02122dbc = { (void (*)())func_ov074_021203e0 };

/* Size-indexed material parameters, in ROM order. Retail words are
 * 0x00d00000/0x00500080 here and 0x003c0059/0x0001001e below. */
unsigned short data_ov074_02122dfc[4] = {0x0000, 0x00d0, 0x0080, 0x0050};
unsigned short data_ov074_02122e04[4] = {0x0059, 0x003c, 0x001e, 0x0001};

KuriKingFreeState data_ov074_02122e0c = { (void (*)())func_ov074_021201ec };

KuriKingStateTable data_ov074_021230f8;

/* The two registry factories close the unit (ov074 .text 0x02122764..
 * 0x0212290c), KURIKING_VANISH first. Both construct the same 0x610-byte
 * class; the registry profile selects the actor variant. */

/* Reconstructed source-style name: SM64DS proves daKuriKing_c through RTTI,
 * allocation size, vtable identity, and the KURIKING_VANISH registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: ExplosionGoomba_Spawn. */
daKuriKing_c *daKuriKing_c_classInit_KURIKING_VANISH(void) {
    return new daKuriKing_c;
}

/* Reconstructed source-style name: SM64DS proves daKuriKing_c through RTTI,
 * allocation size, vtable identity, and the KURIKING registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Goomboss_Spawn. */
daKuriKing_c *daKuriKing_c_classInit_KURIKING(void) {
    return new daKuriKing_c;
}
extern KrbAnimationFileHandle data_ov084_02130ce8;
extern KrbAnimationFileHandle data_ov084_02130ce0;
extern KrbAnimationFileHandle data_ov084_02130cd0;
extern KrbAnimationFileHandle data_ov084_02130cc0;
extern KrbAnimationFileHandle gKuriboRecoverAnimationFile;
extern KrbAnimationFileHandle data_ov084_02130cf0;

SharedFilePtr *const sKurikingMinionAnimationFiles[7] = {
    &data_ov084_02130ce8,         &data_ov084_02130ce0, &data_ov084_02130cd0, &data_ov084_02130cc0,
    &gKuriboRecoverAnimationFile, &data_ov084_02130cc8, &data_ov084_02130cf0,
};

SharedFilePtr *const sKurikingAnimationFiles[12] = {
    &gKurikingAngerAnimationFile,     &gKurikingDamageBAnimationFile,
    &gKurikingDamageFAnimationFile,   &gKurikingDownStartAnimationFile,
    &gKurikingDownAnimationFile,      &gKurikingSerchWaitAnimationFile,
    &gKurikingTurnAnimationFile,      &data_ov074_02123030,
    &gKurikingWalkAnimationFile,      &gKurikingWalkEndAnimationFile,
    &gKurikingWalkStartAnimationFile, &gKurikingSpawnAnimationFile,
};

/* The two registry entries share the class, with distinct factories and flags.
 * SpawnInfo field names describe the framework consumers, not original names. */
SpawnInfo<daKuriKing_c> g_profile_KURIKING = {daKuriKing_c_classInit_KURIKING,
                                              0x00c6,
                                              0x0016,
                                              0x34800000,
                                              0,
                                              0x00100000,
                                              0x03000000,
                                              0x03000000};

SpawnInfo<daKuriKing_c> g_profile_KURIKING_VANISH = {daKuriKing_c_classInit_KURIKING_VANISH,
                                                     0x00c7,
                                                     0x0017,
                                                     0x24800002,
                                                     0,
                                                     0x00100000,
                                                     0x03000000,
                                                     0x03000000};
