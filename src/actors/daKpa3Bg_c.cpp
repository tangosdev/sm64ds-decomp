//cpp
/* daKpa3Bg_c -- KOOPA3BG, a dBgActor_c in ov060. The class name is the ROM's
 * own RTTI spelling (evidence in include/daKpa3Bg_c.h). The tree used to call
 * it BowserSkyPlatform.
 *
 * ROM span 0x02117d1c..0x02118438: D1, D0, three helpers, CleanupResources,
 * Render, Behavior, InitResources, the mesh-collider callback
 * func_ov060_021183cc and its veneer func_ov060_021183f4, then the registry
 * factory daKpa3Bg_c_classInit. Source order is the reverse of the ROM. The destructor is under `#pragma opt_propagation on`
 * and `#pragma defer_codegen off`, so it is emitted as it is parsed (D1, D0,
 * then a D2 the cartridge has no home for) before the deferred functions.
 * `#pragma opt_propagation off` is last in the file: deferred codegen uses that
 * setting, which is what func_ov060_02117db8 matched under. Do not move it.
 *
 * Behavior dispatches data_ov060_0211b1ac[mState] as pointer-to-member
 * records (a function address and a zero adjustment, copied by sinit from
 * ov060 0x0211a930/938/940). The helpers are declared members so &daKpa3Bg_c::
 * func_ov060_... is a real PMF constant, but defined as free functions under
 * their literal mangled spellings: func_ov060_02117db8's body only matches
 * under `#pragma cplusplus off`, which has no member syntax.
 *
 * Leftover:
 * - The three helpers keep their ROM-address names.
 * - func_ov060_021180e0 reads three bytes of the daKpa actor that daKpa_c.h
 *   does not name: the word at +0x410, the word at +0x418 (tested against
 *   0x10000) and the signed byte at +0x41e. mState at +0x40c is the real field.
 * - func_ov060_02117db8 still calls Earthquake and PoofDustAt through their
 *   mangled names. Earthquake is not declared on dActor_c.h. It also calls
 *   func_02012694(0xbc) at the saved camera-space position; that symbol has
 *   no recovered name here.
 * - dBgW_KcMbg::SetFile stays a mangled bridge (Fix12 by value).
 * - func_020393d4 / func_020393c4 stay address-taking calls. The first stores
 *   dBgW::UpdatePosWithTransform; the second stores func_ov060_021183f4.
 * - data_ov060_02119564 / 02119568 / 0211956c are one 0xc-stride table split
 *   into three symbols (x, z, angle). Each reference stays, so the
 *   relocations keep their destinations.
 * - What mState values 1 and 2 mean, beyond which helper the table calls, is
 *   not recovered. Neither is variant's model pair, nor daKpa mState 0xd / 3.
 */

#include "common.h"
#include "daKpa3Bg_c.h"
#include "SharedFilePtr.h"

struct BMD_File;
struct KCL_File;
struct CLPS_Block;

extern "C" {

typedef struct Vec3 { int x, y, z; } Vec3;

void Vec3_AsrInPlace(Vec3 *v, int n);
void MulVec3Mat4x3(Vec3 *in, void *m, void *out);
void func_02012694(int a, void *p);
void CopyTexPalFromLevelModel(void *p);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, Vec3 *v, int fix);
void _ZN8dActor_c10PoofDustAtERK7Vector3(void *self, Vec3 *v);
void _ZN7fBase_c18MarkForDestructionEv(void *self);

extern u16 data_ov060_0211956c[];
extern s16 data_02082214[];
extern int data_ov060_02119564[];
extern int data_ov060_02119568[];
extern int data_0209b3ec;

extern SharedFilePtr *data_ov060_02119514[];
extern SharedFilePtr *data_ov060_0211953c[];
extern CLPS_Block *data_ov060_0211a980[];
extern int data_0208e738;

void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);
void func_020393d4(void *p, void *v);
void func_020393c4(void *p, void *v);
void _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_();
void func_ov060_021183f4(void *collider, daKpa3Bg_c *self, dActor_c *other);
void func_ov060_021183cc(daKpa3Bg_c *self, dActor_c *other);

// local extern: defined below as a free function (cplusplus off has no member syntax); the header declares the member spelling for the PMF table.
void _ZN10daKpa3Bg_c19func_ov060_02117db8Ev(char *self);
// local extern: defined below as a free function (cplusplus off has no member syntax); the header declares the member spelling for the PMF table.
void _ZN10daKpa3Bg_c19func_ov060_021180e0Ev(char *c);
// local extern: defined below as a free function (cplusplus off has no member syntax); the header declares the member spelling for the PMF table.
int _ZN10daKpa3Bg_c19func_ov060_021181b4Ev(char *c);

}

typedef void (daKpa3Bg_c::*Handler)();
struct HandlerEntry { Handler pmf; };
extern HandlerEntry data_ov060_0211b1ac[];

/* The ten model handles construct through func_02017acc and destroy through
 * func_02017ab4; the ten collision handles construct through func_02017b4c
 * and destroy through SharedFilePtr_Destruct_Clsn. The wrappers are declared,
 * never defined: the manifest aliases their members onto the ROM veneers. */
struct Kpa3BgModelFilePtr : SharedFilePtr {
    u32 words[2];

    Kpa3BgModelFilePtr(u32 fileID);
    ~Kpa3BgModelFilePtr();
};

struct Kpa3BgCollisionFilePtr : SharedFilePtr {
    u32 words[2];

    Kpa3BgCollisionFilePtr(u32 fileID);
    ~Kpa3BgCollisionFilePtr();
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- daKpa3Bg_c_classInit, 0x02118408, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol daKpa3Bg_c_classInit
/* Reconstructed source-style name: SM64DS proves daKpa3Bg_c through RTTI,
 * allocation size, vtable identity, and the KOOPA3BG registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: BowserSkyPlatform_Spawn.
 *
 * `new daKpa3Bg_c` is the whole sequence the loose factory spelled by hand:
 * fBase_c::operator new(0x32c), dBgActor_c's base constructor and the vptr
 * store (the class adds no member with a constructor). */
extern "C" daKpa3Bg_c *daKpa3Bg_c_classInit(void)
{
    return new daKpa3Bg_c;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov060_021183f4, 0x021183f4, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_021183f4
/* dBgW callback veneer, armed by func_020393c4 in InitResources. Drops the
   collider and forwards the platform and the touching actor into
   func_ov060_021183cc. */
extern "C" void func_ov060_021183f4(void *collider, daKpa3Bg_c *self, dActor_c *other)
{
    func_ov060_021183cc(self, other);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov060_021183cc, 0x021183cc, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov060_021183cc
/* Flags the platform when the actor that touched it is Bowser
   (actor ID 0x117); Behavior clears the flag every frame. The comparison
   is materialised into an int before the test, as the ROM does. */
extern "C" void func_ov060_021183cc(daKpa3Bg_c *self, dActor_c *other)
{
    int isKoopa = other->actorID == 0x117;
    if (isKoopa != 0)
        self->mTouchedKoopa = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN10daKpa3Bg_c13InitResourcesEv, 0x021182b0, size 0x11c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daKpa3Bg_c13InitResourcesEv
s32 daKpa3Bg_c::InitResources()
{
    mVariant = (u8)(param1 & 0xf);
    data_0208e738 = 0;
    mModel.SetFile(
        (BMD_File *)Model::LoadFile(*data_ov060_02119514[mVariant]), 1, -1);
    CopyTexPalFromLevelModel(&mModel.data);
    data_0208e738 = 1;
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(*data_ov060_0211953c[mVariant]),
        mClsnMat, 0x1000, mAngleY,
        *data_ov060_0211a980[mVariant]);
    func_020393d4(&mMeshCollider,
                  (void *)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4(&mMeshCollider, (void *)&func_ov060_021183f4);
    mMeshCollider.Enable(this);
    mKoopaUniqueId = 0;
    mTouchedKoopa = 0;
    mState = 0;
    mActive = 0;
    mPhase = 0;
    mTimer = mPhase;
    mVertAccel = 0;
    mTerminalVelocity = -0x1e000;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN10daKpa3Bg_c8BehaviorEv, 0x02118254, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daKpa3Bg_c8BehaviorEv
s32 daKpa3Bg_c::Behavior()
{
    (this->*(data_ov060_0211b1ac[mState].pmf))();
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mTouchedKoopa = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN10daKpa3Bg_c6RenderEv, 0x0211822c, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daKpa3Bg_c6RenderEv
s32 daKpa3Bg_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN10daKpa3Bg_c16CleanupResourcesEv, 0x021181e8, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daKpa3Bg_c16CleanupResourcesEv
s32 daKpa3Bg_c::CleanupResources()
{
    mMeshCollider.Disable();
    data_ov060_02119514[mVariant]->Release();
    data_ov060_0211953c[mVariant]->Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov060_021181b4, 0x021181b4, size 0x34 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daKpa3Bg_c19func_ov060_021181b4Ev
/* Finds the daKpa actor (id 0x117) and remembers its uniqueID. */
extern "C" int _ZN10daKpa3Bg_c19func_ov060_021181b4Ev(char *c)
{
    daKpa3Bg_c *bg = (daKpa3Bg_c *)c;
    dActor_c *koopa = dActor_c::FindWithActorID(0x117, 0);
    if (koopa) {
        bg->mKoopaUniqueId = koopa->uniqueID;
        bg->mState = 1;
        return 1;
    }
    return (int)koopa;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov060_021180e0, 0x021180e0, size 0xd4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daKpa3Bg_c19func_ov060_021180e0Ev
/* Leftover: naming mKoopaUniqueId / mTimer / mState here changed 7 words
   (measured). The body stays the offset form that matches. */
extern "C" {
void* _ZN8dActor_c10FindWithIDEj(unsigned int);
void _ZN10daKpa3Bg_c19func_ov060_021180e0Ev(char* c){
  char* a;
  a=(char*)_ZN8dActor_c10FindWithIDEj(*(unsigned int*)(c+0x320));
  if(a==0){ ((fBase_c*)c)->MarkForDestruction(); return; }
  if(*(unsigned char*)(c+0x32b)!=0 && *(int*)(a+0x40c)==0xd){
    if((*(int*)(a+0x418)&0x10000)!=0) *(unsigned char*)(c+0x328)=2;
  }
  if(*(signed char*)(a+0x41e)==1){
    if(*(int*)(a+0x40c)==3 || *(int*)(a+0x410)!=0){
      *(unsigned char*)(c+0x32a)=1;
    }
  }
  if(*(unsigned char*)(c+0x32a)==0){
    *(short*)(c+0x324)=0;
    return;
  }
  {
    int idx=*(unsigned char*)(c+0x329)*0x14;
    unsigned short* p=(unsigned short*)(c+0x324);
    if(idx<*(unsigned short*)(c+0x300+0x24)) *(unsigned char*)(c+0x328)=2;
    *p+=1;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov060_02117db8, 0x02117db8, size 0x328 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN10daKpa3Bg_c19func_ov060_02117db8Ev
/* Leftover: the same body compiled as C++ changed 89 words and two relocation
   destinations (data_ov060_02119564 versus data_02082214). It parses as C.
   int yimm is declared with the other locals: the C front end inside a C++
   TU rejects a declaration after a statement. */
#pragma cplusplus off
void _ZN10daKpa3Bg_c19func_ov060_02117db8Ev(char *self) {
    volatile int saved[3];
    int v[6];
    Vec3 eq;
    Vec3 dust1;
    Vec3 dust2;
    u16 state;
    int live = 2;
    int off;
    int ang;
    s16 mul;
    int bx, bz, k;
    int x, y, z;
    int a;
    int *p;
    int *py;

    state = *(u16 *)(self + 0x326);
    if (state == 0 || state == 0x16) {
        saved[0] = *(int *)(self + 0x74);
        saved[1] = *(int *)(self + 0x78);
        {
        int yimm;
        saved[2] = *(int *)(self + 0x7c);
        yimm = 0x12c000;
        v[0] = *(int *)(self + 0x5c);
        v[1] = *(int *)(self + 0x60);
        v[2] = *(int *)(self + 0x64);
        off = *(u8 *)(self + 0x329) * 0xc;
        ang = *(u16 *)((char *)data_ov060_0211956c + off);
        v[1] = yimm;
        v[0] = (s16)data_02082214[(ang >> 4) * 2] * (s16)0x366
            + (*(int *)((char *)data_ov060_02119564 + off) << 12);
        v[2] = (s16)data_02082214[(ang >> 4) * 2 + 1] * (s16)0x366
            + (*(int *)((char *)data_ov060_02119568 + *(u8 *)(self + 0x329) * 0xc) << 12);
        Vec3_AsrInPlace((Vec3 *)v, 3);
        MulVec3Mat4x3((Vec3 *)v, &data_0209b3ec, self + 0x74);
        func_02012694(0xbc, self + 0x74);
        *(int *)(self + 0x74) = saved[0];
        *(int *)(self + 0x78) = saved[1];
        *(int *)(self + 0x7c) = saved[2];
        }
    }

    if ((u16)*(u16 *)(self + 0x326) >= 0x16u) {
        *(int *)(self + 0x9c) = -0x4000;
    } else {
        eq.x = *(int *)(self + 0x5c);
        eq.y = *(int *)(self + 0x60);
        eq.z = *(int *)(self + 0x64);
        _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(self, &eq, 0x7d0000);
        *(int *)(self + 0xa8) = 0x8000;
        *(int *)(self + 0x9c) = 0;
    }

    state = *(u16 *)(self + 0x326);
    if (state % 2 == 0 && (u16)state < 0xeu) {
        off = *(u8 *)(self + 0x329) * 0xc;
        mul = *(s16 *)((char *)data_ov060_0211956c + off);
        k = (6 - ((int)state >> 1)) * 0x122;
        bx = *(int *)((char *)data_ov060_02119564 + off);
        bz = *(int *)((char *)data_ov060_02119568 + off);

        a = ((int)(u16)(s16)(mul + 0x14b0) >> 4) * 2;
        x = k * (s16)data_02082214[a] + (bx << 12);
        y = 0x133000;
        z = k * (s16)data_02082214[a + 1] + (bz << 12);
        v[3] = x;
        v[4] = y;
        v[5] = z;
        dust1.x = x;
        dust1.y = y;
        dust1.z = z;
        _ZN8dActor_c10PoofDustAtERK7Vector3(self, &dust1);

        a = ((int)(u16)(s16)(mul - 0x14b0) >> 4) * 2;
        x = k * (s16)data_02082214[a] + (bx << 12);
        y = 0x133000;
        z = k * (s16)data_02082214[a + 1] + (bz << 12);
        v[3] = x;
        v[4] = y;
        v[5] = z;
        dust2.x = x;
        dust2.y = y;
        dust2.z = z;
        _ZN8dActor_c10PoofDustAtERK7Vector3(self, &dust2);
    }

    {
        int *pa = (int *)(((int)self + 0xa8));
        int *pypos = (int *)(((int)self + 0x60));
        *pa = *pa + *(int *)(self + 0x9c);
        if (*(int *)(self + 0xa8) <= *(int *)(self + 0xa0))
            *(int *)(self + 0xa8) = *(int *)(self + 0xa0);
        *pypos = *pypos + *(int *)(self + 0xa8);
    }
    if ((u16)*(u16 *)(self + 0x326) > 0x12cu)
        _ZN7fBase_c18MarkForDestructionEv(self);
    (*(u16 *)(((int)self + 0x326)))++;
    (void)live;
}
#pragma cplusplus on

// @symbol _ZN10daKpa3Bg_cD1Ev
// @symbol _ZN10daKpa3Bg_cD0Ev
#pragma opt_propagation on
#pragma defer_codegen off
daKpa3Bg_c::~daKpa3Bg_c()
{
}
#pragma defer_codegen on
#pragma opt_propagation off

/* The static-init globals -- mwcc emits __sinit_daKpa3Bg_c.cpp from these:
 * one ctor veneer plus destructor registration per file handle, then the
 * three pointer-to-member records copied into the dispatch table. Order
 * matches the retail initializer exactly. */
Kpa3BgModelFilePtr data_ov060_0211b06c(0x625);
Kpa3BgModelFilePtr data_ov060_0211b034(0x627);
Kpa3BgModelFilePtr data_ov060_0211b0a4(0x629);
Kpa3BgModelFilePtr data_ov060_0211b03c(0x62b);
Kpa3BgModelFilePtr data_ov060_0211b0b4(0x62d);
Kpa3BgModelFilePtr data_ov060_0211b044(0x62f);
Kpa3BgModelFilePtr data_ov060_0211b094(0x631);
Kpa3BgModelFilePtr data_ov060_0211b09c(0x633);
Kpa3BgModelFilePtr data_ov060_0211b054(0x635);
Kpa3BgModelFilePtr data_ov060_0211b024(0x637);
Kpa3BgCollisionFilePtr data_ov060_0211b0ac(0x626);
Kpa3BgCollisionFilePtr data_ov060_0211b02c(0x628);
Kpa3BgCollisionFilePtr data_ov060_0211b05c(0x62a);
Kpa3BgCollisionFilePtr data_ov060_0211b064(0x62c);
Kpa3BgCollisionFilePtr data_ov060_0211b07c(0x62e);
Kpa3BgCollisionFilePtr data_ov060_0211b01c(0x630);
Kpa3BgCollisionFilePtr data_ov060_0211b074(0x632);
Kpa3BgCollisionFilePtr data_ov060_0211b084(0x634);
Kpa3BgCollisionFilePtr data_ov060_0211b08c(0x636);
Kpa3BgCollisionFilePtr data_ov060_0211b04c(0x638);
HandlerEntry data_ov060_0211b1ac[3] = {
    { &daKpa3Bg_c::func_ov060_021181b4 },
    { &daKpa3Bg_c::func_ov060_021180e0 },
    { &daKpa3Bg_c::func_ov060_02117db8 },
};
