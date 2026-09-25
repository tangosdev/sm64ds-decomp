//cpp
/* Production translation unit for ov080/daBttBk_c.
 * 20 function(s), .text 0x02124a20..0x021253b4.
 *
 * The BATTA_BLOCK crate. InitResources drops it onto the ground below its
 * spawn point and enters state 0, where it falls and bounces; landing on a
 * carrier actor (stored at +0x374) moves it to state 1, where it rides that
 * actor's angles and matrix. A Mega Mario hit (02124acc), or the flag test
 * in state 1 (02124edc), breaks it into dust and five coins.
 *
 * NAME: daBttBk_c is the cartridge's RTTI spelling -- _ZTS at ov080
 * 0x0212815c is the byte string "9daBttBk_c", and _ZTI at 0x02128168 reads
 * [__si_class_type_info vtable (0x0209a764), that string, _ZTI8dActor_c
 * (0x0208e390)]. The vtable's offset-to-top word (0x02128190) is 0 and its
 * RTTI word (0x02128194) is that _ZTI; the address point _ZTV9daBttBk_c is
 * 0x02128198 (slot 0, InitResources). The class was previously the coined
 * name CrazedCrate.
 *
 * THE DESTRUCTOR IS THE KEY FUNCTION, declared first in daBttBk_c.h and
 * defined first below, so this TU emits _ZTV9daBttBk_c and the RTTI chain
 * as vague linkage. Under `#pragma defer_codegen off` mwccarm emits each
 * function as it is parsed, so the file is written in ROM-ascending order
 * and the out-of-line destructor comes out D1 (0x02124a20), D0
 * (0x02124a68), then a D2 the cartridge has no home for (deadstripped;
 * vtable slots 16/17 hold D1 and D0 only). D0's deallocation is an inline
 * operator delete, which is why nothing below mentions a heap.
 *
 * THE STATE MACHINE. The eleven func_ov080_* functions of this run keep
 * their address names: the cartridge preserves no spelling for them. The
 * ROM ties each one to this class instead of a name --
 *   - the six state bodies are the pointer-to-member constants at ov080
 *     0x0212812c..0x02128158, the .data words directly before
 *     _ZTS9daBttBk_c. __sinit_ov080_02127a60 constructs this class's model
 *     file data_ov080_02128468 and copies those constants into the 3-row
 *     state table data_ov080_0212847c (.bss): state 0 = {0212509c enter,
 *     0212500c update}, state 1 = {02124fec, 02124edc}, state 2 =
 *     {02124eb0, 02124e60}. Each enter function stores its own index at
 *     +0x370;
 *   - 0212513c points +0x36c at a row and runs its enter function through
 *     02125104; 021250c8 runs the row's update function from Behavior;
 *   - 02124acc (collision reaction, from state 0's update) and 02124c3c
 *     (matrices and drop shadow, from Behavior and InitResources) have no
 *     callers outside this run.
 * All eleven lie between OnYoshiTryEat (0x02124ac4) and CleanupResources
 * (0x02125158) with no gap, and nothing outside this run and its PMF
 * constants references any of them.
 *
 * Leftover: func_ov080_02124eb0's old one-function source called it
 *   "MontyMole_Kill" (daChoropu_c::Kill); the state table makes it this
 *   class's state-2 enter function, not a daChoropu_c method.
 * Leftover: Particle::System::NewSimple, dActor_c::SpawnCoins and
 *   dActor_c::DropShadowScaleXYZ stay mangled extern "C" calls -- each
 *   takes Fix12<int> by value, and a member call homes the argument and
 *   changes the ROM ABI.
 * Leftover: the factory daBttBk_c_classInit (0x021253b4; reconstructed
 *   name, historical alias CrazedCrate_Spawn) sits just past this run's
 *   right edge and stays in src/d_a_btt_bk.c.
 */

#include "decl_common.h"
#include "daBttBk_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "Player.h"

struct Mtx43 { int a[12]; };
typedef char Mtx43_size_must_be_0x30[sizeof(Mtx43) == 0x30 ? 1 : -1];

struct C;
typedef void (C::*PMF)();
struct C { char pad[0x36c]; PMF *pp; };
typedef char C_size_must_be_0x370[sizeof(C) == 0x370 ? 1 : -1];

extern "C" {
void *func_02010304(void *a, void *b);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const Vector3 &v, unsigned int n, int vel, short unk);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3 &pos);
void Matrix4x3_ApplyInPlaceToTranslation(Mtx43 *m, int x, int y, int z);
void Vec3_LslInPlace(Vector3 *v, int sh);
void Matrix4x3_FromRotationY(void *m, int angle);
void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *sm, void *mtx, int r, int t5, int t6, unsigned int u);
void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *w);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v, int c);
extern void func_ov080_02124c3c(char *c);
}

extern Mtx43 data_020a0e68;
extern s16 data_02082214[];
extern char data_ov080_0212847c[];

/* InitResources and func_ov080_02124e60 leave r1 holding the 0 they just
 * stored. A two-arg call would write r1 again. */
typedef void (*Fn2513c1)(char *);

#pragma defer_codegen off

// @symbol _ZN9daBttBk_cD1Ev
// @symbol _ZN9daBttBk_cD0Ev
/* Empty on purpose. mwccarm destroys the members, then ~dActor_c, and
 * emits retail D1 followed by D0. */
daBttBk_c::~daBttBk_c()
{
}

// @symbol _ZN9daBttBk_c13OnYoshiTryEatEv
s32 daBttBk_c::OnYoshiTryEat()
{
    return 1;
}

/* Collision reaction, run from state 0's update: a carrier found by
 * func_02010304 is stored at +0x374 and enters state 1; otherwise, against
 * actor id 0xbf, mFlags bit 17 enters state 2 and a +0x16c bit 4 contact
 * breaks the crate (Player::IncMegaKillCount, dust, five coins). */
// @symbol func_ov080_02124acc
extern "C" void func_ov080_02124acc(char *c)
{
    if (*(int *)(c + 0x170) == 0) return;
    void *p = func_02010304(c, c + 0x14c);
    if (p != 0) { *(void **)(c + 0x374) = p; func_ov080_0212513c(c, 1); return; }
    void *a = dActor_c::FindWithID(*(unsigned int *)(c + 0x170));
    if (a == 0) return;
    int b1 = (int)(*(unsigned short *)((char *)a + 0xc) == 0xbf);
    if (b1 == 0) return;
    int b2 = (int)((*(unsigned int *)(c + 0xb0) & 0x20000) != 0);
    if (b2) { func_ov080_0212513c(c, 2); return; }
    if ((*(unsigned int *)(c + 0x16c) & 0x10) == 0) return;
    ((Player *)a)->IncMegaKillCount();
    Vector3 v; Vector3 v2; Vector3 v3;
    int y0 = *(int *)(c + 0x60);
    int z = *(int *)(c + 0x64);
    int x = *(int *)(c + 0x5c);
    int y = y0 + 0x32000;
    v.x = x; v.y = y; v.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, v.x, v.y, v.z);
    ((int *)&v2)[0] = ((int *)&v)[0];
    ((int *)&v2)[1] = ((int *)&v)[1];
    ((int *)&v2)[2] = ((int *)&v)[2];
    ((dActor_c *)c)->PoofDustAt(v2);
    int t = *(int *)(c + 0x60) + 0x64000;
    v3.x = v.x;
    v.y = t;
    v3.y = t;
    v3.z = v.z;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, v3, 5, 0xf000, 0);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3 *)(c + 0x74));
    ((fBase_c *)c)->MarkForDestruction();
}

/* Model and shadow matrices: follows the carrier's matrix while carried,
 * then rebuilds the model matrix at +0xf0 and the drop shadow at +0x33c. */
// @symbol func_ov080_02124c3c
extern "C" void func_ov080_02124c3c(char *c)
{
    Vector3 t;
    Vector3 pos;
    int flags = *(int *)(c + 0xb0);
    int gb = (flags & 0x40000) != 0;
    if (gb != false) return;
    void *p = *(void **)(c + 0x374);
    if (p != 0) {
        int gb2 = (flags & 0x4000) != 0;
        if (gb2 != false) {
            if (*(int *)((char *)p + 0xc8) != 0) {

                int tx = 0xc000, ty = 0x2000, tz = 0;
                ((int *)&t)[0] = tx; ((int *)&t)[1] = ty; ((int *)&t)[2] = tz;
                p = *(void **)(c + 0x374);
                data_020a0e68 = *(Mtx43 *)(*(void **)((char *)p + 0xc8));
                Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((int *)&t)[0], ((int *)&t)[1], ((int *)&t)[2]);

                *(int *)(c + 0x5c) = data_020a0e68.a[9];
                *(int *)(c + 0x60) = data_020a0e68.a[10];
                *(int *)(c + 0x64) = data_020a0e68.a[11];
                Vec3_LslInPlace((Vector3 *)(c + 0x5c), 3);
            }
        }
    }
    Matrix4x3_FromRotationXYZExt(c + 0xf0, *(s16 *)(c + 0x8c), *(s16 *)(c + 0x8e), *(s16 *)(c + 0x90));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x11c) = *(int *)(c + 0x64) >> 3;
    pos.x = *(int *)(c + 0x5c);
    pos.y = *(int *)(c + 0x60);
    pos.z = *(int *)(c + 0x64);
    pos.y = pos.y + 0x14000;
    dBgCh_Gnd rg;
    rg.SetObjAndPos(pos, 0);
    int h = pos.y;
    if (rg.DetectClsn()) {
        h = rg.clsnY;
    }
    Matrix4x3_FromRotationY(c + 0x33c, *(s16 *)(c + 0x8e));
    *(int *)(c + 0x360) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x364) = h >> 3;
    *(int *)(c + 0x368) = *(int *)(c + 0x64) >> 3;
    {
        s16 a = *(s16 *)(c + 0x8c);
        int sv = data_02082214[((unsigned short)(short)(a << 1) >> 4) * 2];
        if (sv < 0) sv = -sv;
        int result = (int)(((s64)sv * 0x28000 + 0x800) >> 12);
        _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            c, c + 0x124, c + 0x33c, 0x96000, 0x32000, result + 0x96000, 0xf);
    }
}

/* State 2 update: back to state 0 once neither flag bit 17 nor 18 is set.
 * The one-argument call selects state 0 through the r1 left at 0. */
// @symbol func_ov080_02124e60
extern "C" int func_ov080_02124e60(char *c)
{
    int v = *(int *)(c + 0xb0);
    int b1 = (v & 0x20000) ? 1 : 0;
    if (b1 != 0) goto done;
    int b2 = (v & 0x40000) ? 1 : 0;
    if (b2 != 0) goto done;
    *(int *)(c + 0xd0) = 0;
    ((Fn2513c1)func_ov080_0212513c)(c);
done:
    return 1;
}

/* State 2 enter. */
// @symbol func_ov080_02124eb0
extern "C" int func_ov080_02124eb0(char *c)
{
    *(int *)(c + 0x98) = 0;
    ((dCc_c *)((char *)c + 0x14c))->Clear();
    *(int *)(c + 0x370) = 2;
    return 1;
}

/* State 1 update: copies the carrier's angles; breaks the crate unless
 * mFlags bit 8 is set and bit 13 clear. */
// @symbol func_ov080_02124edc
extern "C" int func_ov080_02124edc(char *c)
{
    *(short *)(c + 0x8c) = *(short *)(*(char **)(c + 0x374) + 0x8c);
    *(short *)(c + 0x8e) = *(short *)(*(char **)(c + 0x374) + 0x8e);
    *(short *)(c + 0x92) = *(short *)(c + 0x8c);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    {
        int f = *(int *)(c + 0xb0);
        int a = (int)((f & 0x100) != 0);
        if (a != 0) {
            int b = (int)((f & 0x2000) != 0);
            if (b == 0) goto clear;
        }
        {
            Vector3 vec;
            Vector3 vec2;
            int x = *(int *)(c + 0x5c);
            int z = *(int *)(c + 0x64);
            int y = *(int *)(c + 0x60) + 0xb4000;
            vec.x = x;
            vec.y = y;
            vec.z = z;
            ((int *)&vec2)[0] = ((int *)&vec)[0];
            ((int *)&vec2)[1] = ((int *)&vec)[1];
            ((int *)&vec2)[2] = ((int *)&vec)[2];
            ((dActor_c *)c)->PoofDustAt(vec2);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, vec.x, vec.y, vec.z);

            {
                int ybase = *(int *)(c + 0x60);
                int xx = vec.x;
                int y2 = ybase + 0x64000;
                int zz = vec.z;
                Vector3 v3;
                v3.x = xx;
                v3.z = zz;
                vec.y = y2;
                v3.y = y2;
                _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, v3, 5, 0xf000, 0);
            }

            _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3 *)(c + 0x74));
            ((fBase_c *)c)->MarkForDestruction();
        }
    }
clear:
    ((dCc_c *)(c + 0x14c))->Clear();
    return 1;
}

/* State 1 enter. */
// @symbol func_ov080_02124fec
extern "C" int func_ov080_02124fec(int *c)
{
    int *r = (int *)(((int)c + 0xb0));
    int one = 1;
    *(int *)((char *)c + 0x370) = one;
    *r &= ~3;
    return one;
}

/* State 0 update: fall, bounce at 60% on landing, collide. */
// @symbol func_ov080_0212500c
extern "C" int func_ov080_0212500c(char *c)
{
    dBgCh_Actr_UpdateDiscreteNoLava_veneer((dBgCh_Actr *)(c + 0x180));
    if (((dBgCh_Actr *)(c + 0x180))->JustHitGround()) {
        int v = *(int *)(c + 0xa8) * -0x3c;
        *(int *)(c + 0xa8) = v / 100;
    } else if (((dBgCh_Actr *)(c + 0x180))->IsOnGround()) {
        *(int *)(c + 0xa8) = 0xc000;
    }
    ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x14c));
    func_ov080_02124acc(c);
    ((dCc_c *)(c + 0x14c))->Clear();
    ((dCc_c *)(c + 0x14c))->Update();
    return 1;
}

/* State 0 enter. */
// @symbol func_ov080_0212509c
extern "C" int func_ov080_0212509c(char *c)
{
    *(int *)(c + 0xa8) = 49152;
    ((dBgCh_Actr *)((char *)c + 0x180))->SetLimMovFlag();
    *(int *)(c + 0x370) = 0;
    return 1;
}

/* Runs the current state row's update function (second PMF). */
// @symbol func_ov080_021250c8
extern "C" void func_ov080_021250c8(char *raw)
{
    C *c = (C *)raw;
    PMF *p = c->pp + 1;
    (c->**p)();
}

/* Runs the current state row's enter function (first PMF). */
// @symbol func_ov080_02125104
extern "C" void func_ov080_02125104(C *c)
{
    PMF *p = c->pp;
    (c->**p)();
}

/* Changes state: points +0x36c at row i of the state table, then enters it. */
// @symbol func_ov080_0212513c
extern "C" void func_ov080_0212513c(char *c, int i)
{
    *(char **)(c + 0x36c) = data_ov080_0212847c + (i << 4);
    func_ov080_02125104((C *)c);
}

/* Slot 3: one shared model file handle to give back. */
// @symbol _ZN9daBttBk_c16CleanupResourcesEv
s32 daBttBk_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov080_02128468)->Release();
    return 1;
}

/* Slot 12: empty override. */
// @symbol _ZN9daBttBk_c16OnPendingDestroyEv
void daBttBk_c::OnPendingDestroy()
{
}

// @symbol _ZN9daBttBk_c6RenderEv
int daBttBk_c::Render()
{
    int r1 = mFlags;
    r1 = r1 & 0x40000;
    r1 = r1 ? 1 : 0;
    if (r1) return 1;
    mModel.Render(0);
    return 1;
}

// @symbol _ZN9daBttBk_c8BehaviorEv
int daBttBk_c::Behavior()
{
    func_ov080_021250c8((char *)this);
    func_ov080_02124c3c((char *)this);
    return 1;
}

// @symbol _ZN9daBttBk_c13InitResourcesEv
int daBttBk_c::InitResources()
{
    Vector3 pos;
    void *file = Model::LoadFile(*(SharedFilePtr *)&data_ov080_02128468);
    mModel.SetFile((BMD_File *)file, 1, 1);
    mShadowModel.InitCuboid();
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x14c, ((char *)this), 0x64000, 0x78000, 0x800004, 0x9010);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    {
        int p60;
        pos.x = mPosX;
        p60 = mPosY;
        pos.y = p60;
        pos.z = mPosZ;
        pos.y = p60 + 0xc8000;
    }
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x180, ((char *)this), 0x32000, 0x32000, 0, 0);
    unk_374 = 0;
    ((Fn2513c1)func_ov080_0212513c)((char *)this);
    func_ov080_02124c3c((char *)this);
    return 1;
}

// @symbol _ZN9daBttBk_c13OnTurnIntoEggER6Player
void daBttBk_c::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap()) {
        GivePlayerCoins(player, 5, 0);
    }
    Vector3 vec;
    Vector3 vec2;
    int x = mPosX;
    int z = mPosZ;
    int y = mPosY + 0xb4000;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    ((int *)&vec2)[0] = ((int *)&vec)[0];
    ((int *)&vec2)[1] = ((int *)&vec)[1];
    ((int *)&vec2)[2] = ((int *)&vec)[2];
    PoofDustAt(vec2);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, vec.x, vec.y, vec.z);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}
