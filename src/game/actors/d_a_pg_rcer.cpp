//cpp
/**
 * daPgRcer_c -- the racing penguin of Cool, Cool Mountain (PENGUIN_RACER),
 * ov019. RTTI ov019:0x021132dc is the string 10daPgRcer_c.
 *
 * He talks the player into a race, runs his path with rubber-band speed
 * (func_ov019_02111254), and catches the shortcut: once the player's
 * airborne fall adds up past 0x7d0000 (mFallAccum), mPlayerCheated is set.
 *
 * DO NOT "TIDY" THESE -- each one is load-bearing:
 *
 *   mwccarm emits ordinary .text in reverse source order; keep the factory
 *   first. The inline destructor in daPgRcer_c emits retail D1/D0 and no D2.
 *
 *   `#pragma opt_common_subs off` is file-global (last one wins). Dropping it
 *   changes the bytes of the race helpers (measured on the shadow TU).
 *
 *   ((int)c + 0x300) + 0x8c for mTargetAngY in the path helpers is the
 *   matching addressing form.
 *
 *   (Vector3 *)&mScaleX in Render and (Vector3 *)&mPosX in InitResources'
 *   GetNode: dActor_c has no Pos() accessor.
 *
 * WHY SOME CALLS ARE SPELLED AS MANGLED SYMBOLS:
 *   Fix12<int> by value (notes/mwccarm-codegen.md 6az): SetAnim,
 *   TextureSequence::SetFile, dCcAc_c::Init, dBgCh_Actr::Init, SetRanges,
 *   DropShadowRadHeight
 *   (InitResources and the race helpers). The dBgCh Init header's Fix12i
 *   mangles as int.
 *   Not in dBgCh_Actr.h: GetFloorResult / GetWallResult
 *   (func_ov019_0211140c).
 *
 * Known limits:
 *   The helpers keep their func_ov019_* labels (PMF dispatch and race
 *   states). func_02038414 is the UpdateDiscreteNoLava_2 veneer
 *   (func_ov019_0211140c); func_0201267c / func_02012790 are unnamed.
 *
 * NOT OWNED BY THIS TU: the data_ov019_* handles and the finish/cheat
 * volumes are constructed by the overlay's sinit; g_profile_PENGUIN_RACER
 * lives elsewhere.
 */

#pragma opt_common_subs off

#include "daPgRcer_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "SurfaceInfo.h"

typedef struct { int x, y, z; } Vec3;
#define LB(off) (*(unsigned char *)(((int)c + (off))))

extern "C" {

extern SharedFilePtr data_ov019_02113498;
extern SharedFilePtr *data_ov019_02112788[7];
extern SharedFilePtr *data_ov019_0211277c[3];
extern daPgRcerState data_ov019_0211356c[];
extern Vector3 data_ov019_021134e8;
extern Vector3 data_ov019_0211353c;
extern int data_ov019_02113460[];
extern int data_ov019_02113468[];
extern int data_ov019_02113470[];
extern int data_ov019_02113478[];
extern int data_ov019_02113480[];
extern int data_ov019_02113488[];
extern int data_ov019_02113490[];
extern int data_ov019_021134a0[];
extern int data_ov019_021134a8[];
extern int data_ov019_021134b0[];
extern unsigned char data_0209d684;

extern unsigned char NumStars(void);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void func_0201267c(int a, void *pos);
extern void func_02012790(int x);
extern void func_02038414(void *c);
extern int _ZN4cstd4fdivEii(int a, int b);
extern Fix12i Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern short Vec3_HorzAngle(void *a, void *b);
extern int Vec3_Dist(void *a, void *b);
extern void _Z11UpdateAngleRssis(short *a, short b, int n, short s);
extern int _Z14ApproachLinearRsss(short *p, short target, short step);
extern void _Z14ApproachLinearRiii(int *p, int a, int b);
extern void Matrix4x3_FromRotationY(void *m, short angle);

/* Fix12-by-value, notes/mwccarm-codegen.md 6az. This TU's InitResources / race
   helpers. */
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *, void *, int, int, unsigned int, unsigned int);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *, void *, int, int, void *, int);
extern void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void *self, int a, int b, int cc, int d);
extern void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void *a, void *sm, void *mtx, int rad, int h, unsigned int x);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *btp, int a, int fix, unsigned int b);
extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *c);
extern void *_ZNK10dBgCh_Actr13GetWallResultEv(void *c);

/* Player talk helpers: reference-vs-pointer is unprovable (this TU). */

extern void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int cc, int d, int e);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 cc, void *v, u32 d);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int cc, int d, int e, void *f, void *g);

int func_ov019_02111254(void *unused, int playerLead);
int func_ov019_0211127c(void *self, Vector3 *v, unsigned int j);
void func_ov019_021112b8(void *self);
int func_ov019_0211131c(void *self);
void func_ov019_021113b0(void *self);
int func_ov019_0211140c(int *self, void *clsn);
void func_ov019_021114ec(void *c);
void func_ov019_02112268(char *c);
void func_ov019_021122a4(void *self);
void func_ov019_021122dc(void *c, int s);

}

extern int _ZTV10daPgRcer_c[];

/* Reconstructed source-style name. SM64DS proves daPgRcer_c through RTTI,
 * allocation size, vtable identity, and the PENGUIN_RACER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: RacingPenguin_Spawn.
 * `return new daPgRcer_c()` MATCHES; the synthesized ctor stores
 * `_ZTV10daPgRcer_c[2]`. */
// @symbol daPgRcer_c_classInit
extern "C" daPgRcer_c *daPgRcer_c_classInit()
{
    return new daPgRcer_c();
}

// @symbol _ZN10daPgRcer_c13InitResourcesEv
int daPgRcer_c::InitResources()
{
    unsigned char *c = (unsigned char *)((void *)this);
    int i;

    mModelAnim.SetFile(
        (BMD_File *)Model::LoadFile(*(SharedFilePtr *)&data_ov019_02113498), 1, 1);

    for (i = 0; i < 7; i++)
        Animation::LoadFile(*(SharedFilePtr *)data_ov019_02112788[i]);

    for (int j = 0; j < 3; j++) {
        void *t = data_ov019_0211277c[j];
        TextureSequence::LoadFile(*(SharedFilePtr *)t);
        TextureSequence::Prepare(
            *(BMD_File *)((void **)&data_ov019_02113498)[1],
            *(BTP_File *)((void **)t)[1]);
    }

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    if (NumStars() == 0x96) {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0x174, c, 0xd0000, 0x12c000, 0x800004, 0);
        mScaleX = 0x1999;
        mScaleY = 0x1000;
        mScaleZ = 0x1999;
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(c + 0x1a8, c, 0xd0000, 0xd0000, 0, 0);
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(c, 0xf0000, 0xf0000, 0x1c20000, 0x1c20000);
    } else {
        _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(c + 0x174, c, 0x82000, 0x12c000, 0x800004, 0);
        mScaleX = 0x1000;
        mScaleY = 0x1000;
        mScaleZ = 0x1000;
        _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(c + 0x1a8, c, 0x82000, 0x82000, 0, 0);
    }

    mStarSlot = (u8)TrackStar((u8)((param1 >> 8) & 0xf), 2);
    func_ov019_021122dc(c, 0);
    mPath.FromID(param1 & 0xff);
    mPathNodeIndex = 0;
    mPath.GetNode(*(Vector3 *)&mPosX, mPathNodeIndex);
    func_ov019_021113b0(c);
    func_ov019_021114ec(c);
    return 1;
}

// @symbol _ZN10daPgRcer_c8BehaviorEv
int daPgRcer_c::Behavior()
{
    func_ov019_02112268((char *)this);
    mModelAnim.Animation::Advance();
    mTextureSequence.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    func_ov019_021114ec((char *)this);
    return 1;
}

// @symbol _ZN10daPgRcer_c6RenderEv
int daPgRcer_c::Render()
{
    mTextureSequence.Update(mModelAnim.data);
    mModelAnim.Render((Vector3 *)&mScaleX);
    return 1;
}

// @symbol _ZN10daPgRcer_c16OnPendingDestroyEv
void daPgRcer_c::OnPendingDestroy()
{
}

// @symbol _ZN10daPgRcer_c16CleanupResourcesEv
int daPgRcer_c::CleanupResources()
{
    data_ov019_02113498.Release();
    for (int i = 0; i < 7; i++)
        data_ov019_02112788[i]->Release();
    for (int i = 0; i < 3; i++)
        data_ov019_0211277c[i]->Release();
    return 1;
}

extern "C" {

// @symbol func_ov019_021122dc
void func_ov019_021122dc(void *vself, int i)
{
    daPgRcer_c *self = (daPgRcer_c *)vself;
    self->mStateDesc = &data_ov019_0211356c[i];
    func_ov019_021122a4(self);
}

// @symbol func_ov019_021122a4
void func_ov019_021122a4(void *vc)
{
    daPgRcer_c *c = (daPgRcer_c *)vc;
    daPgRcerStateMethod *p = &c->mStateDesc->init;
    (c->**p)();
}

// @symbol func_ov019_02112268
void func_ov019_02112268(char *vc)
{
    daPgRcer_c *c = (daPgRcer_c *)vc;
    daPgRcerStateMethod *p = &c->mStateDesc->behavior;
    (c->**p)();
}

// @symbol func_ov019_021121f8
int func_ov019_021121f8(char *c)
{
    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0xa0) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0xd4, (void *)data_ov019_02113470[1], 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((char *)c + 0x138, (void *)data_ov019_02113468[1], 0, 0x1000, 0);
    *(int *)(c + 0x374) = 0;
    return 1;
}

// @symbol func_ov019_02112168
int func_ov019_02112168(char *c)
{
    if (DecIfAbove0_Byte((unsigned char *)c + 0x38e))
        return 1;
    void *pl = ((daPgRcer_c *)c)->ClosestPlayer();
    if (pl == 0)
        return 1;
    int d178 = *(int *)((char *)c + 0x178);
    if (Vec3_Dist((char *)c + 0x5c, (char *)pl + 0x5c) < d178 + 0x78000) {
        if (((Player *)pl)->StartTalk(*(fBase_c *)c, 1)) {
            *(int *)((char *)c + 0x378) = (int)pl;
            func_ov019_021122dc(c, 1);
        }
    }
    return 1;
}

// @symbol func_ov019_0211213c
int func_ov019_0211213c(char *self)
{
    extern int func_0201267c(int a, void *pos, int b);
    char *pos = self + 0x74;
    *(unsigned char *)(self + 0x38f) = 0;
    func_0201267c(0xdf, pos, 0);
    *(int *)(self + 0x374) = 1;
    return 1;
}

// @symbol func_ov019_02111fec
int func_ov019_02111fec(char *c)
{
    switch (*(unsigned char *)(c + 0x38f)) {
    case 0: {
        char *tgt = *(char **)(c + 0x378);
        short ang = Vec3_HorzAngle((struct Vector3 *)(c + 0x5c), (struct Vector3 *)(tgt + 0x5c));
        if (_Z14ApproachLinearRsss((short *)(c + 0x8e), ang, 0x514) != 0) {
            struct Vector3 pos;
            int msg;
            int eq;
            int y;
            int z;
            pos.x = *(int *)(c + 0x5c);
            y = *(int *)(c + 0x60);
            pos.y = y;
            z = *(int *)(c + 0x64);
            pos.z = z;
            pos.y = y + 0x190000;
            eq = (int)(NumStars() == 0x96);
            if (eq != 0)
                msg = 0xab;
            else
                msg = 0xa7;
            if (((Player *)(*(void **)(c + 0x378)))->ShowMessage(*(fBase_c *)c, (unsigned int)(short)msg, &pos, 1, 2) != 0) {
                LB(0x38f) = LB(0x38f) + 1;
            }
        }
        break;
    }
    case 1:
        if (((Player *)(*(void **)(c + 0x378)))->GetTalkState() == 2) {
            unsigned char d = data_0209d684;
            if (d == 1) {
                func_02012790(0x98);
                func_ov019_021122dc(c, 2);
            } else if (d == 2) {
                func_02012790(0x63);
                *(unsigned char *)(c + 0x38e) = 0x5a;
                ((Player *)(*(void **)(c + 0x378)))->HasFinishedTalking();
                func_ov019_021122dc(c, 0);
            }
        }
        break;
    }
    return 1;
}

// @symbol func_ov019_02111f54
int func_ov019_02111f54(char *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0xd4, (void *)data_ov019_02113478[1], 0x40000000, 0x1000, 0);
    *(int *)((char *)c + 0x9c) = -0x2000;
    *(int *)((char *)c + 0xa0) = -0x3c000;
    *(int *)((char *)c + 0xa8) = 0x16000;
    *(unsigned char *)((char *)c + 0x38f) = 0;
    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x14, 0x7f, 0x15666, 0);
    func_0201267c(0x138, (char *)c + 0x74);
    *(int *)((char *)c + 0x374) = 2;
    return 1;
}

// @symbol func_ov019_02111dec
int func_ov019_02111dec(char *c)
{
    switch (*(unsigned char *)(c + 0x38f)) {
    case 0:
        if (((Player *)(*(void **)(c + 0x378)))->Unk_020c4f40(0x5a) != 0) {
            LB(0x38f) = LB(0x38f) + 1;
        }
        break;
    case 1:
        _Z11UpdateAngleRssis(
            (short *)(c + 0x94),
            *(short *)((char *)(((int)c + 0x300)) + 0x8c),
            2, 0x800);
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x174));
        func_ov019_0211140c((int *)c, c + 0x1a8);
        if (((dBgCh_Actr *)(c + 0x1a8))->JustHitGround() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov019_02113460[1], 0x40000000, 0x1000, 0);
            LB(0x38f) = LB(0x38f) + 1;
        }
        break;
    case 2:
        if (((Animation *)(c + 0x124))->Finished() != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov019_02113470[1], 0, 0x1000, 0);
        }
        if (((Player *)(*(void **)(c + 0x378)))->GetTalkState() == -1) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1f, 0x7f, 0, 0x15666, 0);
            func_0201267c(0x4d, c + 0x74);
            func_ov019_021122dc(c, 3);
        }
        break;
    }
    return 1;
}

// @symbol func_ov019_02111d58
int func_ov019_02111d58(char *c)
{
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0xd4, (void *)data_ov019_02113480[1], 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((char *)c + 0x138, (void *)data_ov019_021134b0[1], 0, 0x1000, 0);
    *(int *)(c + 0x98) = 0x30000;
    *(char *)(c + 0x38f) = 0;
    *(char *)(c + 0x393) = 0;
    *(int *)(c + 0x374) = 3;
    return 1;
}

// @symbol func_ov019_0211197c
int func_ov019_0211197c(void *self)
{
    u8 *c = (u8 *)self;

    switch (c[0x38f]) {
    case 0:
        if (func_ov019_0211131c(c)) {
            func_ov019_021113b0(c);
        }
        if (((Animation *)(c + 0x124))->Finished()) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov019_02113488[1], 0, 0x1000, 0);
            *(int *)(c + 0x130) = 0x1000;
            c[0x38f]++;
        }
        ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x174));
        func_ov019_0211140c((int *)c, c + 0x1a8);
        if (c[0x394] == 0 && c[0x393] == 0) {
            {
                Vector3 v;
                v.x = *(int *)(c + 0x5c);
                v.y = *(int *)(c + 0x60);
                v.z = *(int *)(c + 0x64);
                if (func_ov019_0211127c(c, &v, c[0x390])) {
                    c[0x390]++;
                }
            }
            {
                u8 *o = *(u8 **)(c + 0x378);
                u8 *p = (u8 *)(((int)o + 0x5c));
                Vector3 v;
                v.x = *(int *)p;
                v.y = *(int *)(p + 4);
                v.z = *(int *)(p + 8);
                if (func_ov019_0211127c(c, &v, c[0x391])) {
                    c[0x391]++;
                }
            }
        }
        break;

    case 1:
        if (func_ov019_0211131c(c)) {
            if (*(int *)(c + 0x36c) >= (int)((PathPtr *)(c + 0x364))->NumNodes() - 2) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov019_02113490[1], 0x40000000, 0x1000, 0);
                c[0x38f]++;
            } else {
                func_ov019_021113b0(c);
            }
        }
        ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x174));
        func_ov019_0211140c((int *)c, c + 0x1a8);
        if (c[0x394] == 0 && c[0x393] == 0 &&
            *(int *)(c + 0x36c) < (int)((PathPtr *)(c + 0x364))->NumNodes() - 2) {
            {
                Vector3 v;
                v.x = *(int *)(c + 0x5c);
                v.y = *(int *)(c + 0x60);
                v.z = *(int *)(c + 0x64);
                if (func_ov019_0211127c(c, &v, c[0x390])) {
                    c[0x390]++;
                }
            }
            {
                u8 *o = *(u8 **)(c + 0x378);
                u8 *p = (u8 *)(((int)o + 0x5c));
                Vector3 v;
                v.x = *(int *)p;
                v.y = *(int *)(p + 4);
                v.z = *(int *)(p + 8);
                if (func_ov019_0211127c(c, &v, c[0x391])) {
                    c[0x391]++;
                }
            }
        }
        if (c[0x391] > 1) {
            int val = func_ov019_02111254(c, c[0x391] - c[0x390]);
            if (c[0x394] != 0 || c[0x393] != 0) {
                val = 0x65000;
            }
            _Z14ApproachLinearRiii((int *)(c + 0x98), val, 0x800);
        }
        if (c[0x392] == 0 && Vec3_Dist(&data_ov019_021134e8, (Vector3 *)(c + 0x5c)) < 0x190000) {
            if (c[0x393] != 0) {
                *(u8 *)(c + 0x395) = 0;
            } else {
                *(u8 *)(c + 0x395) = 1;
            }
            *(u8 *)(c + 0x392) = 1;
        }
        *(u32 *)(c + 0x384) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(u32 *)(c + 0x384), 3, 0x185, c + 0x74, 0);
        *(void **)(c + 0x388) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32 *)(c + 0x388), 0x101, *(int *)(c + 0x5c), *(int *)(c + 0x60) + 0x32000, *(int *)(c + 0x64), 0, 0);
        break;

    case 2:
        if (((Animation *)(c + 0x124))->Finished()) {
            func_ov019_021122dc(c, 4);
        }
        break;
    }

    func_ov019_021112b8(c);
    if (Vec3_Dist(&data_ov019_0211353c, (Vector3 *)(*(u8 **)(c + 0x378) + 0x5c)) < 0x280000) {
        *(u8 *)(c + 0x394) = 1;
    }
    if (Vec3_Dist(&data_ov019_021134e8, (Vector3 *)(*(u8 **)(c + 0x378) + 0x5c)) < 0x190000) {
        *(u8 *)(c + 0x393) = 1;
    }
    return 1;
}

// @symbol func_ov019_02111904
int func_ov019_02111904(char *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0xd4, (void *)data_ov019_021134a0[1], 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj((char *)c + 0x138, (void *)data_ov019_02113468[1], 0, 0x1000, 0);
    *(char *)(c + 0x38f) = 0;
    *(int *)(c + 0x98) = 0x5000;
    *(int *)(c + 0x374) = 4;
    return 1;
}

// @symbol func_ov019_021117a8
int func_ov019_021117a8(char *c)
{
    int node[3];
    switch (*(unsigned char *)(c + 0x38f)) {
    case 0: {
        int n = ((PathPtr *)(c + 0x364))->NumNodes();
        ((PathPtr *)(c + 0x364))->GetNode(*(Vector3 *)node, n - 1);
        *(int *)(c + 0x380) = Vec3_Dist(c + 0x5c, node);
        *(short *)((char *)(((int)c + 0x300)) + 0x8c) =
            Vec3_HorzAngle(c + 0x5c, node);
        _Z14ApproachLinearRsss(
            (short *)(c + 0x8e),
            *(short *)((char *)(((int)c + 0x300)) + 0x8c),
            0x200);
        *(short *)(c + 0x94) = *(short *)(c + 0x8e);
        if (*(int *)(c + 0x380) < *(int *)(c + 0x98)) {
            *(int *)(c + 0x98) = *(int *)(c + 0x380);
            LB(0x38f) = LB(0x38f) + 1;
        }
        ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x174));
        func_ov019_0211140c((int *)c, c + 0x1a8);
        {
            unsigned int s = ((unsigned int)*(int *)(c + 0x12c) << 4) >> 0x10;
            if (s == 9 || s == 0x15) {
                func_0201267c(0xde, c + 0x74);
            }
        }
        break;
    }
    case 1: {
        short ang = Vec3_HorzAngle(c + 0x5c, *(char **)(c + 0x378) + 0x5c);
        if (_Z14ApproachLinearRsss((short *)(c + 0x8e), ang, 0x514) != 0) {
            func_ov019_021122dc(c, 5);
        }
        break;
    }
    }
    {
        unsigned int s = ((unsigned int)*(int *)(c + 0x12c) << 4) >> 0x10;
        if (s == 9 || s == 0x15) {
            func_0201267c(0xf3, c + 0x74);
        }
    }
    return 1;
}

// @symbol func_ov019_02111754
int func_ov019_02111754(char *c)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void *)data_ov019_02113470[1], 0, 0x1000, 0);
    *(int *)(c + 0x130) = 0x1000;
    *(char *)(c + 0x38f) = 0;
    *(int *)(c + 0x374) = 5;
    return 1;
}

// @symbol func_ov019_02111558
int func_ov019_02111558(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;

    switch (*(unsigned char *)(c + 0x38f)) {
    case 0: {
        int radius = *(int *)(c + 0x178);
        int d = Vec3_Dist(c + 0x5c, (char *)*(void **)(c + 0x378) + 0x5c);
        if (d < radius + 0x78000) {
            if (((Player *)(*(void **)(c + 0x378)))->StartTalk(*(fBase_c *)c, 1) != 0) {
                LB(0x38f) = LB(0x38f) + 1;
            }
        }
        break;
    }
    case 1: {
        short ang = Vec3_HorzAngle(c + 0x5c, (char *)*(void **)(c + 0x378) + 0x5c);
        if (_Z14ApproachLinearRsss((short *)(c + 0x8e), ang, 0x514) != 0) {
            unsigned int id;
            Vec3 v;
            int z = *(int *)(c + 0x64);
            int y = *(int *)(c + 0x60) + 0x190000;
            int x = *(int *)(c + 0x5c);
            v.x = x;
            v.y = y;
            v.z = z;
            if (*(unsigned char *)(c + 0x394) != 0) {
                id = 0xa9;
                _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void *)data_ov019_021134a8[1], 0, 0x1000, 0);
                *(unsigned char *)(c + 0x395) = 1;
            } else {
                if (*(unsigned char *)(c + 0x395) != 0)
                    id = 0xaa;
                else
                    id = 0xa8;
            }
            if (((Player *)(*(void **)(c + 0x378)))->ShowMessage(*(fBase_c *)c, id, (Vector3 *)&v, 1, 2) != 0) {
                func_0201267c(0xdf, c + 0x74);
                LB(0x38f) = LB(0x38f) + 1;
            }
        }
        break;
    }
    case 2:
        if (((Player *)(*(void **)(c + 0x378)))->GetTalkState() == 2) {
            if (*(unsigned char *)(c + 0x395) == 0) {
                ((dActor_c *)c)->UntrackAndSpawnStar(*(signed char *)(((int)c + 0x396)), (unsigned int)(unsigned char)((*(unsigned int *)(c + 8) >> 8) & 0xf), *(Vector3 *)(c + 0x5c), 4);
            }
            ((Player *)(*(void **)(c + 0x378)))->HasFinishedTalking();
            _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x138, (void *)data_ov019_02113468[1], 0, 0x1000, 0);
            LB(0x38f) = LB(0x38f) + 1;
        }
        break;
    }
    return 1;
}

// @symbol func_ov019_021114ec
void func_ov019_021114ec(void *c)
{
    char *self = (char *)c;
    Matrix4x3_FromRotationY(self + 0xf0, *(short *)(self + 0x8e));
    *(int *)(self + 0x114) = *(int *)(self + 0x5c) >> 3;
    *(int *)(self + 0x118) = *(int *)(self + 0x60) >> 3;
    *(int *)(self + 0x11c) = *(int *)(self + 0x64) >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(self, self + 0x14c, self + 0xf0, 0x140000, 0x50000, 0xf);
}

// @symbol func_ov019_0211140c
int func_ov019_0211140c(int *self, void *clsn)
{
    int n0[3];
    int n1[3];
    func_02038414(clsn);
    if (((dBgCh_Actr *)clsn)->IsOnGround()) {
        ((SurfaceInfo *)((char *)_ZNK10dBgCh_Actr14GetFloorResultEv(clsn) + 4))->CopyNormalTo(*(Vector3 *)n0);
        if (n0[1] != 0) {
            long long a = (long long)n0[0] * (long long)self[0xa4 / 4];
            long long b = (long long)n0[2] * (long long)self[0xac / 4];
            int x = (int)((a + 0x800) >> 12);
            int y = (int)((b + 0x800) >> 12);
            self[0xa8 / 4] = -(_ZN4cstd4fdivEii(x + y, n0[1]) + 0x8000);
            if (self[0xa8 / 4] > 0)
                self[0xa8 / 4] = 0;
        }
    }
    if (((dBgCh_Actr *)clsn)->IsOnWall()) {
        ((SurfaceInfo *)((char *)_ZNK10dBgCh_Actr13GetWallResultEv(clsn) + 4))->CopyNormalTo(*(Vector3 *)n1);
    }
}

// @symbol func_ov019_021113b0
void func_ov019_021113b0(void *vc)
{
    char *c = (char *)vc;
    struct Vector3 v;
    int *cnt = (int *)(((int)c + 0x36c));
    *cnt = *cnt + 1;
    ((daPgRcer_c *)c)->mPath.GetNode(v, *(int *)(c + 0x36c));
    *(int *)(c + 0x380) = Vec3_Dist((struct Vector3 *)(c + 0x5c), &v);
    *(short *)((char *)(((int)c + 0x300)) + 0x8c) = Vec3_HorzAngle((struct Vector3 *)(c + 0x5c), &v);
}

// @symbol func_ov019_0211131c
int func_ov019_0211131c(void *vc)
{
    char *c = (char *)vc;
    _Z11UpdateAngleRssis(
        (short *)(c + 0x94),
        *(short *)((char *)(((int)c + 0x300)) + 0x8c),
        2, 0x600);
    int n = *(int *)(c + 0x36c);
    if (n >= (int)((daPgRcer_c *)c)->mPath.NumNodes() - 2) {
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    } else {
        struct Vector3 v;
        ((daPgRcer_c *)c)->mPath.GetNode(v, n + 1);
        *(short *)(c + 0x8e) = Vec3_HorzAngle((struct Vector3 *)(c + 0x5c), &v);
    }
    {
        int *pv = (int *)(((int)c + 0x380));
        *pv = *pv - *(int *)(c + 0x98);
    }
    return *(int *)(c + 0x380) < 0 ? 1 : 0;
}

// @symbol func_ov019_021112b8
void func_ov019_021112b8(void *vc)
{
    daPgRcer_c *c = (daPgRcer_c *)vc;
    if (c->mPlayerCheated != 0)
        return;
    Player *t = c->mTalkPlayer;
    if (t->mIsAirborne != 0) {
        int vertSpeed = t->mVertSpeed;
        int terminalVel = t->mTerminalVelocity;
        if (vertSpeed > terminalVel)
            return;
        {
            int d = vertSpeed;
            if (d < 0)
                d = -d;
            c->mFallAccum = c->mFallAccum + d;
            if (c->mFallAccum > 0x7d0000)
                c->mPlayerCheated = 1;
        }
        return;
    }
    c->mFallAccum = 0;
}

// @symbol func_ov019_0211127c
int func_ov019_0211127c(void *vc, Vector3 *arg, unsigned int j)
{
    char *c = (char *)vc;
    Vector3 node;
    ((daPgRcer_c *)c)->mPath.GetNode(node, j);
    Fix12i dist = Vec3_HorzDist(&node, arg);
    return dist <= 0x320000 ? 1 : 0;
}

// @symbol func_ov019_02111254
/* Rubber band: the further ahead the player is, in path nodes, the faster
 * the penguin runs -- clamped to 0x30000..0x65000. */
int func_ov019_02111254(void *unused, int playerLead)
{
    int speed = playerLead * 0x54cc + 0x4a800;

    if (speed > 0x65000) {
        speed = 0x65000;
    }
    if (speed < 0x30000) {
        speed = 0x30000;
    }

    return speed;
}

}
