//cpp
/* ov006 0x020bfec0..0x020c4048, the overlay's first linker unit.
 *
 * No RTTI, vtable or static initialiser names a type in this run.
 * Camera_UpdateMatrices at 0x020c0134 is the one named entry.
 *
 * leftovers:
 * - The other functions keep their address names.
 * - Each function keeps its own namespace. The local layouts and
 *   externs disagree, and one shared view changes codegen.
 * - A char* or void* parameter stays where the typed header form
 *   changes the call.
 */

#include "types.h"
#include "nitro/fx/fx.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "PlayerInput.h"
#include "G3i.h"
#include "MascotWalker.h"
#include "Presenter.h"
#include "Dealer.h"
#include "Mascot.h"
#include "FlowerFace.h"
/* Typed access to a MascotWalker record from the char* helpers below. */
#define WALKER(p) ((MascotWalker *)(p))
/* Typed access to a Presenter record from the char* helpers below. */
#define PRESENTER(p) ((Presenter *)(p))
/* Typed access to a Dealer record from the char* helpers below. */
#define DEALER(p) ((Dealer *)(p))
/* Typed access to a Mascot record from the char* helpers below. */
#define MASCOT(p) ((Mascot *)(p))
/* Typed access to the flower game's Yoshi and petal records. */
#define FLOWERFACE(p) ((FlowerFace *)(p))
#define PETAL(p) ((FallingPetal *)(p))

struct Sound { static void PlayBank2_2D(unsigned int); };
struct Model { int d; void HideMaterial(int, int); void ShowMaterial(int, int); void Render(const Vector3 *); };
struct dExtFrameCtrl_c { int d; int Finished(); int WillHitFrame(int f) const; };
namespace dExtShadowModel_c { void RenderAll(); }
void UpdateAngle(short &, short, int, short);
int ApproachLinear(int &, int, int);
int ApproachLinear2(short &, short, short);
extern "C" {
void MulVec3Mat4x3(void *, void *, void *);
int _ZN4cstd4fdivEii(int, int);
void Matrix4x3_ApplyInPlaceToRotationZ(void *, short);
void Matrix4x3_FromQuaternion(void *, void *);
void MulVec3Mat3x3(void *, void *, void *);
void MulMat4x3Mat4x3(void *, void *, void *);
void _ZN9ModelAnimD1Ev(void *);
void *__cxa_vec_cleanup(void *, int, int, void *);
void func_020169d8(void *, int, unsigned int);
void func_0203cebc(void *, void *, void *, void *);
void _ZN17dExtShadowModel_c8CleanAllEv(void); // local extern: raw char*/void* view of the actor memory; the class header declares the typed form and would change call-site codegen
void AddVec3(void *, void *, void *);
int DotVec3(void *, void *);
void Matrix4x3_ApplyInPlaceToRotationX(void *, short);
void Matrix4x3_ApplyInPlaceToRotationY(void *, short);
void Matrix4x3_FromTranslation(void *, int, int, int);
int NormalizeVec3IfNonZero(void *);
void Quaternion_FromVector3(void *, void *, void *);
void Quaternion_Normalize(void *);
void Quaternion_SLerp(void *, void *, int, void *);
int RandomIntInternal(int *);
void SubVec3(void *, void *, void *);
void Vec3_Add(void *, void *, void *);
short Vec3_HorzAngle(const void *, const void *);
short Vec3_VertAngle(const void *, const void *);
void Vec3_MulScalar(void *, void *, int);
void Vec3_MulScalarInPlace(int *, int);
void Vec3_Sub(void *, void *, void *);
void _Z11UpdateAngleRssis(short *, short, int, short);
int _Z14ApproachLinearRiii(int *, int, int);
int _Z15ApproachLinear2Rsss(short *, short, short);
void _Z13CopyToViewMatPK9Matrix4x3(void *);
void SharedFilePtr_Construct_TexSeq(void *, unsigned int);
void SharedFilePtr_Destruct_Anim(void *);
void SharedFilePtr_Destruct_TexSeq(void *);
void _ZN17dExtShadowModel_c12InitCylinderEv(void *); // local extern: receiver is a char* cursor over actor memory (c + 0x88), not the class object
void _ZN17dExtShadowModel_c9InitModelEP9Matrix4x35Fix12IiES3_S3_j(void *, void *, int, int, int, unsigned int);
void *_ZN17dExtShadowModel_cC1Ev(void *);
void _ZN17dExtShadowModel_cD1Ev(void *);
void _ZN13SharedFilePtr7ReleaseEv(void *);
void _ZN13SharedFilePtr9ConstructEj(void *, unsigned int);
void *_ZN14BlendModelAnimC1Ev(void *);
void _ZN14BlendModelAnimD1Ev(void *);
void _ZN14BlendModelAnim7AdvanceEv(void *);
void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *, void *, int, int, int, unsigned short);
void _ZN15TextureSequence6UpdateER15ModelComponents(void *, void *);
void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *, void *);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *);
void *_ZN15TextureSequenceC1Ev(void *);
void _ZN15TextureSequenceD1Ev(void *);
void _ZN18TextureTransformer6UpdateER15ModelComponents(void *, void *);
void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *, void *);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
void *_ZN18TextureTransformerC1Ev(void *);
void _ZN18TextureTransformerD1Ev(void *);
int _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(int, int, int, int, int, int, int, void *);
void _ZN5Model12HideMaterialEii(void *, int, int);
void _ZN5Model12ShowMaterialEii(void *, int, int);
void _ZN5Model12SetPolygonIDEi(void *, int);
void _ZN5Model6RenderEPK7Vector3(void *, void *);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *);
void *_ZN5ModelC1Ev(void *);
void _ZN5ModelD1Ev(void *);
void _ZN5Sound12PlayBank2_2DEj(unsigned int);
/* local extern: dClipper::Func_020156DC takes a by-value Fix12<int> (the Fix12 wall), so it stays spelled out with scalar arguments. */
void _ZN8dClipper13Func_020156DCEitii(void *, int, int, int, int);
void *_ZN7Vector3D1Ev(void *);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
int _ZN15dExtFrameCtrl_c7AdvanceEv(void *);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
int _ZN15dExtFrameCtrl_c8FinishedEv(void *);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
void *_ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(void *);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
void *_ZN9ModelAnimC1Ev(void *);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, void *, int, int);
/* local extern: the call passes an untyped this, so it cannot use the header method. */
int _ZNK15dExtFrameCtrl_c12WillHitFrameEi(void *, int);
void *__cxa_vec_ctor(void *, int, int, void *, void *);
void func_02012174(int, int);
void func_02016a04(void *, int);
void func_02016a14(void *, int);
void func_02016b24(void *, unsigned int);
void func_02017ab4(void *);
void *func_02017acc(void *, unsigned int);
void func_0202ffec(void *, void *, void *);
void func_0203ccd4(void *, short);
void func_0203cd80(void *, short);
void func_020731dc(void *, void *, void **);
void Camera_ProjectToScreen16(char* r4, void* r1, short* r5);
void Camera_ProjectToScreen(char* r4, void* r1, int* r6, int* r5);
void Camera_UpdateMatrices(void *self);
void MascotWalker_StatePause(char *c);
void MascotWalker_StartPause(int *t);
void MascotWalker_StateWalk(char *c);
void MascotWalker_StartNextPath(char* c);
void MascotWalker_UpdateMatrix(char* thiz);
void MascotWalker_Draw(char *t);
void MascotWalker_Update(void * c);
void MascotWalker_Load(char* thiz);
int MascotWalker_Destruct(char *t);
int MascotWalker_Construct(char *t);
void Camera_InitDealerView(void *self);
void Dealer_ResetMatrices(char* c);
void Dealer_StateWin(char *p);
void Dealer_PlayWin(void *c);
void Dealer_StateLose(char *c);
void Dealer_PlayLose(char *c);
void Dealer_StateSwap(char *c);
void Dealer_StartSwap(int *t);
int Dealer_IsAnimDone(void* a);
int Dealer_IsBusy(int *c);
void Dealer_StateReveal(void *cc);
void Dealer_StartReveal(int *t, int a1, int a2);
void Dealer_StateDeal(char *c);
void Dealer_StartDeal(char *c, short arg1, int arg2);
void Dealer_StateShuffle(char *c);
void Dealer_StartShuffle(char *c, int unused, short a2, int a3);
int Dealer_IsIdleAtRest(char *c);
int Dealer_IsIdle(int* r0);
void Dealer_StateIdle(void);
void Dealer_PlayIdle(char *c);
void Dealer_Draw(void *self);
void Dealer_Update(char* thiz);
int Dealer_Load(char *c);
int Dealer_Destruct(char *t); // local extern: raw char*/void* view of the actor memory; the class header declares the typed form and would change call-site codegen
int Dealer_Construct(char *t); // local extern: raw char*/void* view of the actor memory; the class header declares the typed form and would change call-site codegen
void Camera_InitSlotsView(void *self);
void Mascot_StopLooking(int *p);
void Mascot_LookAt(char* c, int* src);
void Mascot_UpdateMatrix(char* c);
void Mascot_Draw(void *c);
void Mascot_Update(void* a);
void Mascot_Load(char *c);
int Mascot_Destruct(char *t); // local extern: raw char*/void* view of the actor memory; the class header declares the typed form and would change call-site codegen
int Mascot_Construct(char *t);
void Camera_InitPresenterView(void *self);
void Presenter_UpdateMatrix(char* c);
void Presenter_StateSurprisedThenIdle(char *t);
void Presenter_PlayCheerLoop(void *c);
void Presenter_StateLoseStart(void *c);
void Presenter_PlayLose(char *c);
void Presenter_StateSurprisedThenSmile(void *c);
void Presenter_PlayCheer(void *c);
void Presenter_StateMistake(char *t);
void Presenter_PlayMiss(char *c);
void Presenter_StateWin(char *t);
void Presenter_PlayMatch(void *c);
void Presenter_StateIdle(char* c);
void Presenter_PlayIdle(char* c);
void Presenter_ResetPose(char* c);
int Presenter_IsAnimDone(void* a);
int Presenter_IsIdle(void * c);
void Presenter_Draw(char *c);
void Presenter_Update(void * c);
void Presenter_TrackStylus(char* c);
int Presenter_Load(char *c);
int Presenter_Destruct(char *t); // local extern: raw char*/void* view of the actor memory; the class header declares the typed form and would change call-site codegen
int Presenter_Construct(char *t);
void FallingPetal_UpdateMatrix(char* c);
void FallingPetal_Draw(char* c);
void FallingPetal_Update(char* self);
void FallingPetal_Spawn(int *r6, int *r5, int *r4);
void FallingPetal_SetModel(char *t, int a1);
void FlowerFace_StateHate(void);
void FlowerFace_PlayHate(char *p);
void FlowerFace_StateLike(void);
void FlowerFace_PlayLike(char *p);
void FlowerFace_StatePick(int *c);
void FlowerFace_PlayPetalPulled(char *c);
void FlowerFace_StateIdle(void);
void FlowerFace_PlayIdle(char *p);
void Camera_InitFlowerView(void *self);
int FlowerFace_IsIdle(int* c);
void FlowerFace_Reset(void *a);
void FlowerFace_Draw(void *self);
void FlowerFace_Update(char *c);
int FlowerFace_Load(char *c);
int FallingPetal_Destruct(char *t);
void* FlowerFace_Destruct(char* c); // local extern: raw char*/void* view of the actor memory; the class header declares the typed form and would change call-site codegen
void* FlowerFace_Construct(char* c);
int FallingPetal_Construct(char *t);}

namespace s020c402c {
extern "C" {
int FallingPetal_Construct(char *t)
{
    _ZN5ModelC1Ev(t + 0x44);
    return (int)t;
}
}
}

namespace s020c3f54 {
extern "C" {
}

extern "C" void* FlowerFace_Construct(char* c)
{
    __cxa_vec_ctor(c + 8, 0x16, 0x98, (void*)FallingPetal_Construct, (void*)FallingPetal_Destruct);
    _ZN9ModelAnimC1Ev(c + 0xd18);
    func_02017acc(c + 0xd7c, 0x222);
    func_02017acc(c + 0xd84, 0x201);
    _ZN13SharedFilePtr9ConstructEj(c + 0xd8c, 0x224);
    _ZN13SharedFilePtr9ConstructEj(c + 0xd94, 0x223);
    _ZN13SharedFilePtr9ConstructEj(c + 0xd9c, 0x221);
    _ZN13SharedFilePtr9ConstructEj(c + 0xda4, 0x220);
    return c;
}
}

namespace s020c3e70 {
extern "C" {
}
extern "C" void* FlowerFace_Destruct(char* c){
  ((SharedFilePtr *)(c + 0xd7c))->Release();
  ((SharedFilePtr *)(c + 0xd84))->Release();
  ((SharedFilePtr *)(c + 0xd8c))->Release();
  ((SharedFilePtr *)(c + 0xd94))->Release();
  ((SharedFilePtr *)(c + 0xd9c))->Release();
  ((SharedFilePtr *)(c + 0xda4))->Release();
  SharedFilePtr_Destruct_Anim((c + 0xda4));
  SharedFilePtr_Destruct_Anim((c + 0xd9c));
  SharedFilePtr_Destruct_Anim((c + 0xd94));
  SharedFilePtr_Destruct_Anim((c + 0xd8c));
  func_02017ab4((c + 0xd84));
  func_02017ab4((c + 0xd7c));
  _ZN9ModelAnimD1Ev(c + 0xd18);
  __cxa_vec_cleanup(c + 8, 0x16, 0x98, (void *)FallingPetal_Destruct);
  return c;
}
}

namespace s020c3e54 {
extern "C" {
int FallingPetal_Destruct(char *t)
{
    _ZN5ModelD1Ev(t + 0x44);
    return (int)t;
}
}
}

namespace s020c3d88 {
struct SharedFilePtr; struct BMD_File;
struct ModelBase { int d; };
extern "C" int FlowerFace_Load(char *c)
{
    {
        void* bmd = _ZN5Model8LoadFileER13SharedFilePtr((c + 0xd7c));
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)(c + 0xd18), bmd, 1, -1);
    }
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr((c + 0xd8c));
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr((c + 0xd94));
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr((c + 0xd9c));
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr((c + 0xda4));
    FlowerFace_PlayIdle(c);
    int m = (int)_ZN5Model8LoadFileER13SharedFilePtr((c + 0xd84));
    int i = 0;
    int z = i;
    char *p = c + 8;
    for (; i < 0x16; i++) {
        FallingPetal_SetModel(p, m);
        *(int *)(c + 0x48) = z;
        p += 0x98;
        c += 0x98;
    }
    return 1;
}
}

namespace s020c3d18 {
extern "C" {
void FlowerFace_Update(char *c)
{
    int v = *(int*)(c+4);
    void *p = c + (v >> 1);
    int *fn;
    if (v & 1) {
        int *vt = *(int**)p;
        int off = *(int*)c;
        fn = (int*)*(int*)((char*)vt + off);
    } else {
        fn = (int*)*(int*)c;
    }
    ((void(*)(void*))fn)(p);
    _ZN15dExtFrameCtrl_c7AdvanceEv(c + 0xd68);
    int i = 0;
    char *e = c + 8;
    do {
        if (*(int*)(c+0x48) != 0)
            FallingPetal_Update(e);
        i++;
        c += 0x98;
        e += 0x98;
    } while (i < 0x16);
}
}
}

namespace s020c3bf4 {
// @symbol FlowerFace_Draw

extern "C" {
extern "C" { extern int data_ov006_0213aee8[12]; }
}



struct Obj {
    virtual void f0() = 0;
    virtual void f1() = 0;
    virtual void f2() = 0;
    virtual void f3() = 0;
    virtual void f4() = 0;
    virtual void f5(int a) = 0;
};

extern "C" void FlowerFace_Draw(void *self)
{
    char *c = (char*)self;
    struct Vector3 v;
    unsigned int packed;
    int i;

    v.x = 0;
    v.y = 0;
    v.z = 0xfffff008;
    func_0203cd80(&v, 0);
    func_0203ccd4(&v, 0x600);

    packed = (((short)v.x >> 3) & 0x3ff)
           | ((((short)v.y >> 3) & 0x3ff) << 10)
           | ((((short)v.z >> 3) & 0x3ff) << 20);
    *(unsigned int*)0x40004c8 = packed;

    func_02016a14(c + 0xd18, 0x2bff);
    func_02016a04(c + 0xd18, 0x1211);

    *(struct Matrix4x3*)(c + 0xd34) = *(struct Matrix4x3*)data_ov006_0213aee8;

    ((Obj*)(c + 0xd18))->f5(0);

    i = 0;
    {
        char *p = c + 8;
        for (; i < 0x16; i++) {
            if (*(int*)(c + 0x48) != 0)
                FallingPetal_Draw(p);
            c += 0x98;
            p += 0x98;
        }
    }
}
}

namespace s020c3bc8 {
extern "C" {
void FlowerFace_Reset(void *a)
{
  int i = 0;
  char *p = (char *) a;
  for (; i < 22; i++)
  {
    *((int *) (p + 0x48)) = 0;
    p += 0x98;
  }
  FlowerFace_PlayIdle((char *)a);
}
}
}

namespace s020c3b80 {
extern "C" {
extern int pmf_FlowerFace_StateIdle_2[];
int FlowerFace_IsIdle(int* c){
  int* g=pmf_FlowerFace_StateIdle_2;
  int r2=c[0];
  int ip=1;
  if(r2==g[0]){
    if(c[1]==g[1] || r2==0) ip=0;
  }
  return ip==0;
}
}
}

namespace s020c3b2c {
extern "C" {
// @symbol Camera_InitFlowerView
/* Camera_InitFlowerView at 0x020c3b2c
 *
 * dCamera_c preset init: sets eye/target vectors and angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of Camera_InitPresenterView.
 */

struct Matrix4x3_local { int data[12]; };

struct dCamera_c {
    struct Matrix4x3_local viewMat;  /* 0x00 */
    char pad30[0x30];          /* 0x30 */
    struct Matrix4x3_local projMat;  /* 0x60 */
    char pad90[0x10];          /* 0x90 */
    struct Vector3 eye;        /* 0xa0 */
    struct Vector3 target;     /* 0xac */
    short angle;               /* 0xb8 */
};


void Camera_InitFlowerView(struct dCamera_c *self)
{
    self->eye.x = 0x1b000;
    self->eye.y = 0x17600;
    self->eye.z = -0x22f00;
    self->target.x = 0x38000;
    self->target.y = 0x22500;
    self->target.z = -0x51200;
    self->angle = 0xb30;
    Camera_UpdateMatrices(self);
}
}
}

namespace s020c3adc {
extern "C" {
struct S { int w[2]; };
extern "C" { extern struct S pmf_FlowerFace_StateIdle; }
void FlowerFace_PlayIdle(char *p) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p + 0xd18, ((void *)FLOWERFACE(p)->mWaitAnim.file),
                                                0, 0x800, 0);
    *(struct S *)(p + 0x0) = pmf_FlowerFace_StateIdle;
}
}
}

namespace s020c3ad8 {
extern "C" {
void FlowerFace_StateIdle(void)
{
}
}
}

namespace s020c3990 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
struct S8 { int w[2]; };


extern "C" { extern int data_0209e650; }
extern "C" {
extern int pmf_FlowerFace_StatePick[2];
}

void FlowerFace_PlayPetalPulled(char *c)
{
    int i;
    char *p;
    struct { Vec3 pos, vel, pos2, vel2; } b;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd18, ((void *)FLOWERFACE(c)->mPickAnim.file),
                                                0x40000000, 0x800, 0);

    *(int *)(c + 0xd70) = 0;

    p = c;
    for (i = 0; i < 0x16; i++, p += 0x98) {
        if (*(int *)(p + 0x48) == 0) {
            b.pos.x = 0x2000;
            b.pos.y = 0xa000;
            b.pos.z = 0x1000;
            b.vel.x = 0x600;
            b.vel.y = 0xa00;
            b.vel.z = 0xa00;
            b.vel.x += (int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) >> 2;
            b.vel.y += (int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) >> 2;
            b.vel.z += (int)((unsigned int)(RandomIntInternal(&data_0209e650) & ~0x80000000) >> 19) >> 2;
            b.pos2.x = b.pos.x;
            b.pos2.y = b.pos.y;
            b.pos2.z = b.pos.z;
            b.vel2.x = b.vel.x;
            b.vel2.y = b.vel.y;
            b.vel2.z = b.vel.z;
            FallingPetal_Spawn((int *)(c + 8 + i * 0x98), (int *)&b.pos2, (int *)&b.vel2);
            break;
        }
    }

    *(S8 *)c = *(S8 *)&pmf_FlowerFace_StatePick[0];
}
}
}

namespace s020c395c {
extern "C" {
void FlowerFace_StatePick(int *c) {
    if (_ZN15dExtFrameCtrl_c8FinishedEv((char *)c + 0xd68) == 0)
        return;
    FlowerFace_PlayIdle((char *)c);
}
}
}

namespace s020c3908 {
extern "C" {
struct S { int w[2]; };
extern "C" {
extern struct S pmf_FlowerFace_StateLike;
}
void FlowerFace_PlayLike(char *p) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p + 0xd18, ((void *)FLOWERFACE(p)->mLikeAnim.file),
                                                0x40000000, 0x800, 0);
    *(struct S *)(p + 0x0) = pmf_FlowerFace_StateLike;
}
}
}

namespace s020c3904 {
extern "C" {
void FlowerFace_StateLike(void) {}
}
}

namespace s020c38b0 {
extern "C" {
struct S { int w[2]; };
extern "C" {
extern struct S pmf_FlowerFace_StateHate;
}
void FlowerFace_PlayHate(char *p) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p + 0xd18, ((void *)FLOWERFACE(p)->mHateAnim.file),
                                                0x40000000, 0x800, 0);
    *(struct S *)(p + 0x0) = pmf_FlowerFace_StateHate;
}
}
}

namespace s020c38ac {
extern "C" {
void FlowerFace_StateHate(void) {}
}
}

namespace s020c3884 {
extern "C" {
void FallingPetal_SetModel(char *t, int a1)
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(t + 0x44, (void *)a1, 1, -1);
    PETAL(t)->mActive = 0;
}
}
}

namespace s020c3754 {
extern "C" {
extern "C" {
extern int sFallingPetal_Spawn_UnitYGuard;
}
extern "C" {
extern int sFallingPetal_Spawn_UnitYDtorRec;
}
extern "C" {
extern int sFallingPetalAxis[3];
}
extern "C" { extern int data_0209e650; }
extern "C" { extern int data_02092768[4]; }

void FallingPetal_Spawn(int *r6, int *r5, int *r4)
{
    int *n;
    if (!(sFallingPetal_Spawn_UnitYGuard & 1)) {
        n = sFallingPetalAxis;
        n[0] = 0;
        n[1] = 0x1000;
        n[2] = 0;
        func_020731dc(n, (void *)_ZN7Vector3D1Ev, (void **)&sFallingPetal_Spawn_UnitYDtorRec);
        sFallingPetal_Spawn_UnitYGuard |= 1;
    }
    r6[0] = r5[0];
    r6[1] = r5[1];
    r6[2] = r5[2];
    r6[3] = r4[0];
    r6[4] = r4[1];
    r6[5] = r4[2];
    r6[6] = 0;
    r6[7] = -0x40;
    r6[8] = 0;
    *(short *)((char *)r6 + 0x94) = 0xb4;
    r6[9] = sFallingPetalAxis[0];
    r6[10] = sFallingPetalAxis[1];
    r6[11] = sFallingPetalAxis[2];
    func_0203cd80((char *)r6 + 0x24, (short)RandomIntInternal(&data_0209e650));
    r6[12] = data_02092768[0];
    r6[13] = data_02092768[1];
    r6[14] = data_02092768[2];
    r6[15] = data_02092768[3];
    Quaternion_FromVector3((char *)r6 + 0x30, sFallingPetalAxis, (int *)((char *)r6 + 0x24));
    *(int *)((char *)r6 + 0x40) = 1;
}
}
}

namespace s020c35e8 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
struct Quat { int w[4]; };


extern "C" { extern Quat data_02092768; }
extern int data_02082214[];

void FallingPetal_Update(char* self)
{
    Vec3 v0;
    Vec3 v1;
    Vec3 v2;
    Vec3 v3;
    Quat q0;
    Quat q1;
    int idx;
    int val;
    int dot;

    if (_Z15ApproachLinear2Rsss((short*)(self + 0x94), 0, 1) != 0) {
        PETAL(self)->mActive = 0;
        return;
    }
    AddVec3(self, self + 0xc, self);
    PETAL(self)->mAccel.y = -0x40;
    AddVec3(self + 0xc, self + 0x18, self + 0xc);
    idx = ((*(int*)(self + 4) >> 1) << 16) >> 20;
    val = *(short*)&data_02082214[idx];
    v0.x = val;
    v0.y = 0;
    v0.z = val;
    Vec3_MulScalarInPlace((int*)&v0, 0x400);
    Vec3_Add(&v1, self + 0xc, &v0);
    dot = DotVec3(self + 0x24, &v1);
    Vec3_MulScalar(&v2, self + 0x24, dot);
    Vec3_MulScalarInPlace((int*)&v2, 0x300);
    SubVec3(self + 0xc, &v2, self + 0xc);
    v3.x = v1.x;
    v3.y = v1.y;
    v3.z = v1.z;
    if (NormalizeVec3IfNonZero(&v3) == 0) return;
    q1 = data_02092768;
    Quaternion_FromVector3(&q0, self + 0x24, &v3);
    Quaternion_SLerp(&q1, &q0, 0x200, &q1);
    Quaternion_Normalize(&q1);
    func_0202ffec(self + 0x30, &q1, self + 0x30);
}
}
}

namespace s020c35a8 {
// @symbol FallingPetal_Draw
extern "C" {
}

struct Obj {
    virtual void m0();
    virtual void m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual void m5(Vector3* v);
};
extern "C" void FallingPetal_Draw(char* c) {
    FallingPetal_UpdateMatrix(c);
    Vector3 v; v.z=0x1000; v.y=0x1000; v.x=0x1000;
    ((Obj*)(c + 0x44))->m5(&v);
}
}

namespace s020c3528 {
// @symbol FallingPetal_UpdateMatrix
extern "C" {
struct Q{int a,b,c,d;};

extern "C" {
extern struct Vector3 sPetalUpAxis;
}
extern "C" { extern struct Matrix4x3 data_020a0e68; }
void FallingPetal_UpdateMatrix(char* c){
  struct Matrix4x3 sp;
  Matrix4x3_FromQuaternion((char*)c+0x30, &sp);
  MulVec3Mat3x3(&sPetalUpAxis, &sp, (char *)c + 0x24);
  Matrix4x3_FromTranslation(&data_020a0e68, *(int*)c, *(int*)(c+4), *(int*)(c+8));
  MulMat4x3Mat4x3(&sp, &data_020a0e68, &data_020a0e68);
  *(struct Matrix4x3*)((char*)c+0x60) = data_020a0e68;
}
}
}

namespace s020c33dc {
extern "C" {
int Presenter_Construct(char *t)
{
    _ZN14BlendModelAnimC1Ev(t + 0x8);
    _ZN5ModelC1Ev(t + 0x78);
    _ZN15TextureSequenceC1Ev(t + 0xc8);
    _ZN18TextureTransformerC1Ev(t + 0xdc);
    func_02017acc(t + 0xf0, 0x286);
    func_02017acc(t + 0xf8, 0x200);
    _ZN13SharedFilePtr9ConstructEj(t + 0x100, 0x1fc);
    _ZN13SharedFilePtr9ConstructEj(t + 0x108, 0x1f6);
    _ZN13SharedFilePtr9ConstructEj(t + 0x110, 0x1fe);
    _ZN13SharedFilePtr9ConstructEj(t + 0x118, 0x1f0);
    _ZN13SharedFilePtr9ConstructEj(t + 0x120, 0x1f4);
    _ZN13SharedFilePtr9ConstructEj(t + 0x128, 0x1f2);
    _ZN13SharedFilePtr9ConstructEj(t + 0x130, 0x1f8);
    _ZN13SharedFilePtr9ConstructEj(t + 0x138, 0x1fa);
    SharedFilePtr_Construct_TexSeq(t + 0x140, 0x1fd);
    SharedFilePtr_Construct_TexSeq(t + 0x148, 0x1f7);
    SharedFilePtr_Construct_TexSeq(t + 0x150, 0x1ff);
    SharedFilePtr_Construct_TexSeq(t + 0x158, 0x1f1);
    SharedFilePtr_Construct_TexSeq(t + 0x160, 0x1f5);
    SharedFilePtr_Construct_TexSeq(t + 0x168, 0x1f3);
    SharedFilePtr_Construct_TexSeq(t + 0x170, 0x1f9);
    SharedFilePtr_Construct_TexSeq(t + 0x178, 0x1fb);
    PRESENTER(t)->mTracking = 0;
    PRESENTER(t)->mQuiet = 0;
    return (int)t;
}
}
}

namespace s020c3288 {
extern "C" {
int Presenter_Destruct(char *t)
{
    _ZN13SharedFilePtr7ReleaseEv(t + 0xf0);
    _ZN13SharedFilePtr7ReleaseEv(t + 0xf8);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x100);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x108);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x110);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x118);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x120);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x128);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x130);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x138);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x140);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x148);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x150);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x158);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x160);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x168);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x170);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x178);
    SharedFilePtr_Destruct_TexSeq(t + 0x178);
    SharedFilePtr_Destruct_TexSeq(t + 0x170);
    SharedFilePtr_Destruct_TexSeq(t + 0x168);
    SharedFilePtr_Destruct_TexSeq(t + 0x160);
    SharedFilePtr_Destruct_TexSeq(t + 0x158);
    SharedFilePtr_Destruct_TexSeq(t + 0x150);
    SharedFilePtr_Destruct_TexSeq(t + 0x148);
    SharedFilePtr_Destruct_TexSeq(t + 0x140);
    SharedFilePtr_Destruct_Anim(t + 0x138);
    SharedFilePtr_Destruct_Anim(t + 0x130);
    SharedFilePtr_Destruct_Anim(t + 0x128);
    SharedFilePtr_Destruct_Anim(t + 0x120);
    SharedFilePtr_Destruct_Anim(t + 0x118);
    SharedFilePtr_Destruct_Anim(t + 0x110);
    SharedFilePtr_Destruct_Anim(t + 0x108);
    SharedFilePtr_Destruct_Anim(t + 0x100);
    func_02017ab4(t + 0xf8);
    func_02017ab4(t + 0xf0);
    _ZN18TextureTransformerD1Ev(t + 0xdc);
    _ZN15TextureSequenceD1Ev(t + 0xc8);
    _ZN5ModelD1Ev(t + 0x78);
    _ZN14BlendModelAnimD1Ev(t + 0x8);
    return (int)t;
}
}
}

namespace s020c3050 {
extern "C" {
extern "C" {
extern char sPresenterLoseLoopTexAnim[];
extern char sPresenterSmileTexAnim[];
extern char sPresenterLoseTexAnim[];
extern char sPresenterIdleTexAnim[];
extern char sPresenterMissTexAnim[];
extern char sPresenterTextureAnim[];
extern char sPresenterCheerTexAnim[];
extern char sPresenterMatchTexAnim[];
}
// @symbol Presenter_Load
extern "C" { extern struct Matrix4x3 data_020a0e68; }



int Presenter_Load(char *c)
{
    void *r5;
    void *r4;

    r5 = _ZN5Model8LoadFileER13SharedFilePtr(c + 0xf0);
    r4 = _ZN5Model8LoadFileER13SharedFilePtr(c + 0xf8);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x100);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x108);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x110);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x118);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x120);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x128);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x130);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x138);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x140);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x148);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x150);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x158);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x160);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x168);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x170);
    _ZN15TextureSequence8LoadFileER13SharedFilePtr(c + 0x178);

    if (!_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 8, r5, 1, -1))
        return 0;

    func_02016b24(c + 8, 2);

    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, ((void *)PRESENTER(c)->mTexSeqs[0].file));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, ((void *)PRESENTER(c)->mTexSeqs[1].file));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, ((void *)PRESENTER(c)->mTexSeqs[2].file));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, ((void *)PRESENTER(c)->mTexSeqs[3].file));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, ((void *)PRESENTER(c)->mTexSeqs[4].file));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, ((void *)PRESENTER(c)->mTexSeqs[5].file));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, ((void *)PRESENTER(c)->mTexSeqs[6].file));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, ((void *)PRESENTER(c)->mTexSeqs[7].file));

    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, sPresenterIdleTexAnim);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, sPresenterMissTexAnim);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, sPresenterMatchTexAnim);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, sPresenterLoseLoopTexAnim);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, sPresenterLoseTexAnim);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, sPresenterTextureAnim);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, sPresenterCheerTexAnim);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, sPresenterSmileTexAnim);

    Presenter_PlayIdle(c);
    PRESENTER(c)->mPos.x = 0;
    PRESENTER(c)->mPos.y = 0;
    PRESENTER(c)->mPos.z = 0;
    Presenter_UpdateMatrix(c);

    if (!_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x78, r4, 1, -1))
        return 0;

    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    *(struct Matrix4x3 *)(c + 0x94) = data_020a0e68;
    return 1;
}
}
}

namespace s020c2be8 {
extern "C" {

#define COMPUTE(c, s, t) \
    { \
        int q = ((s) << 12) / 255; \
        int r = ((t) << 12) / 192; \
        int w = 0x1000 - r; \
        int a; \
        int b; \
        if (q > 0x800) { \
            int d = (q - 0x800) << 1; \
            a = FX_MUL(d, -0x100); \
            b = FX_MUL(d, -0x260); \
        } else { \
            int d = (0x800 - q) << 1; \
            a = FX_MUL(d, 0x260); \
            b = FX_MUL(d, 0x100); \
        } \
        { \
            int m = FX_MUL(r, 0x20) + FX_MUL(w, 0x500); \
            int o1 = *(int*)((c) + 0x18c); \
            int o2 = *(int*)((c) + 0x194); \
            b -= o2; \
            int o3 = *(int*)((c) + 0x190); \
            int o4 = *(int*)((c) + 0x198); \
            *(int*)((c) + 0x18c) += (a - o1) >> 2; \
            *(int*)((c) + 0x194) += b >> 2; \
            o3 = m - o3; \
            *(int*)((c) + 0x190) += o3 >> 2; \
            o4 = m - o4; \
            *(int*)((c) + 0x198) += o4 >> 2; \
        } \
    }

void Presenter_TrackStylus(char* c)
{
    if (PRESENTER(c)->mTracking != 0) {
        int s = PRESENTER(c)->mTouchX;
        int t = PRESENTER(c)->mTouchY;
        COMPUTE(c, s, t);
        return;
    }
    if (Presenter_IsIdle(c) != 0) {
        int i = gActivePlayerSlot;
        if (gTouchHeld[i * 4] != 0) {
            int s = gTouchX[i * 4];
            int t = gTouchY[i * 4];
            COMPUTE(c, s, t);
            return;
        }
    }
    PRESENTER(c)->mSway[0] = FX_MUL(PRESENTER(c)->mSway[0], 0xE00);
    PRESENTER(c)->mSway[1] = FX_MUL(PRESENTER(c)->mSway[1], 0xE00);
    PRESENTER(c)->mSway[2] = FX_MUL(PRESENTER(c)->mSway[2], 0xE00);
    PRESENTER(c)->mSway[3] = FX_MUL(PRESENTER(c)->mSway[3], 0xE00);
}
}
}

namespace s020c2b8c {
extern "C" {
struct C;
typedef void (C::*PMF)();
struct C { PMF pmf; };
void Presenter_Update(C* c) {
    Presenter_TrackStylus((char *)c);
    if (c->pmf) (c->*(c->pmf))();
    _ZN15dExtFrameCtrl_c7AdvanceEv((char*)c + 0xc8);
    _ZN15dExtFrameCtrl_c7AdvanceEv((char*)c + 0xdc);
    _ZN14BlendModelAnim7AdvanceEv((char*)c + 8);
    Presenter_UpdateMatrix((char *)c);
}
}
}

namespace s020c29dc {
extern "C" {
extern "C" {
extern char sPresenterScale;
}
}
struct Obj {
    void* vt;
    virtual void v0(); virtual void v1(); virtual void v2();
    virtual void v3(); virtual void v4();
    virtual void m(void* arg);
};
extern "C" void Presenter_Draw(char *c)
{
    int v0[3];
    int v1[3];
    int *p;
    char *mc;

    v0[0] = 0; v0[1] = 0; v0[2] = 0xfffff008;
    v1[0] = 0; v1[1] = 0; v1[2] = 0xfffff008;
    func_02016a04(c + 8, 0x3d6d);
    func_0203cd80(v0, -0x2000);
    func_0203ccd4(v0, 0x2000);
    func_0203cd80(v1, 0x4200);
    func_0203ccd4(v1, 0x7000);

    *(volatile int *)0x40004cc = 0x7fff;
    *(volatile int *)0x40004c8 =
        (((short)v0[0] >> 3) & 0x3ff) |
        ((((short)v0[1] >> 3) & 0x3ff) << 10) |
        ((((short)v0[2] >> 3) & 0x3ff) << 20);
    mc = c + 0x10;
    *(volatile int *)0x40004cc = 0x4000294b;
    *(volatile int *)0x40004c8 =
        (((short)v1[0] >> 3) & 0x3ff) |
        ((((short)v1[1] >> 3) & 0x3ff) << 10) |
        ((((short)v1[2] >> 3) & 0x3ff) << 20) | 0x40000000;

    p = *(int **)(mc + 4);
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0xc8, mc);
    _ZN18TextureTransformer6UpdateER15ModelComponents(c + 0xdc, mc);
    *(int *)(((int)p + 0x104)) -= PRESENTER(c)->mSway[2];
    *(int *)(((int)p + 0x108)) += PRESENTER(c)->mSway[3];
    *(int *)(((int)p + 0x134)) += PRESENTER(c)->mSway[0];
    *(int *)(((int)p + 0x138)) += PRESENTER(c)->mSway[1];
    ((Obj *)(c + 8))->m(0);
    ((Obj *)(c + 0x78))->m(&sPresenterScale);
}
}

namespace s020c2994 {
extern "C" {
struct P{int x,y;};
extern "C" { extern struct P pmf_Presenter_StateIdle; }
int Presenter_IsIdle(struct P* c){
  int ip=1;
  struct P* g=&pmf_Presenter_StateIdle;
  if(c->x==g->x){
    if(c->y!=g->y && c->x!=0) ;
    else ip=0;
  }
  return ip==0;
}
}
}

namespace s020c2984 {
extern "C" {
/* Presenter_IsAnimDone @ 0x20c2984 (ov006) -- veneer: add r0,r0,#0x58; b dExtFrameCtrl_c::Finished(). */

int Presenter_IsAnimDone(void* a) {
    return _ZN15dExtFrameCtrl_c8FinishedEv((char*)a + 0x58);
}
}
}

namespace s020c2924 {
extern "C" {
extern "C" { extern void* data_0209f5bc; }
void Presenter_ResetPose(char* c){
  Presenter_PlayIdle(c);
  void* obj=data_0209f5bc;
  int (**vt)(void*)=*(int(***)(void*))obj;
  if(vt[6](obj)==0) return;
  PRESENTER(c)->mSway[1] = 0;
  PRESENTER(c)->mSway[0] = PRESENTER(c)->mSway[1];
  PRESENTER(c)->mSway[3] = 0;
  PRESENTER(c)->mSway[2] = PRESENTER(c)->mSway[3];
  *(int*)(c+0x60)=0;
  *(int*)(c+0xd0)=0;
  *(int*)(c+0xe4)=0;
}
}
}

namespace s020c2848 {
extern "C" {
extern "C" {
extern void *sPresenterIdleTexAnim;
}
extern "C" { extern void* data_0209f5bc; }
struct P2 { void* a; void* b; };
extern "C" { extern P2 pmf_Presenter_StateIdle_2; }
}

struct VObj {
    virtual int v0();
    virtual int v1();
    virtual int v2();
    virtual int v3();
    virtual int v4();
    virtual int v5();
    virtual int v6();
};

extern "C" void Presenter_PlayIdle(char* c)
{
    if (PRESENTER(c)->mFaceAnim != PRESENTER(c)->mAnims[0].file) {
        int r5 = 0;
        if (((VObj*)data_0209f5bc)->v6() == 0) {
            if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x58) == 0 ||
                PRESENTER(c)->mFaceAnim == PRESENTER(c)->mAnims[7].file) {
                r5 = 8;
            }
        }
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            c + 8, ((void *)PRESENTER(c)->mAnims[0].file), r5, 0, 0x800, 0);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            c + 0xc8, ((void *)PRESENTER(c)->mTexSeqs[0].file), 0, 0x800, 0);
        _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(
            c + 0xdc, (void *)&sPresenterIdleTexAnim, 0, 0x800, 0);
    }
    {
        void* lo = ((volatile P2*)&pmf_Presenter_StateIdle_2)->a;
        void* hi = ((volatile P2*)&pmf_Presenter_StateIdle_2)->b;
        *(void**)(c) = lo;
        *(void**)(c + 4) = hi;
    }
}
}

namespace s020c27c4 {
extern "C" {
extern "C" {
extern void *sPresenterIdleTexAnim;
}
void Presenter_StateIdle(char* c){
  if (!_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x58)) return;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 8, ((void *)PRESENTER(c)->mAnims[0].file),
                                                     0, 0, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
      c + 0xc8, ((void *)PRESENTER(c)->mTexSeqs[0].file), 0, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0xdc, (void *)&sPresenterIdleTexAnim, 0,
                                                        0x800, 0);
}
}
}

namespace s020c271c {
extern "C" {
extern "C" {
extern int sPresenterMatchTexAnim;
}
extern "C" {
extern long long pmf_Presenter_StateWin;
}
inline char *inline_fn(char *arg0)
{
  return arg0 + 8;
}

void Presenter_PlayMatch(void *c) {
    char *r4 = (char *)c;
    unsigned long long new_var;
    long long *new_var2;
    if ((PRESENTER(r4)->mQuiet) == 0) {
        func_02012174(0, 0x1c);
    }
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        inline_fn(r4), ((void *)PRESENTER(r4)->mAnims[2].file), 4, 0x40000000, 0x800, 0);
    new_var2 = (long long *)r4;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        r4 + 0xc8, ((void *)PRESENTER(r4)->mTexSeqs[2].file), 0x40000000, 0x800, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &sPresenterMatchTexAnim,
                                                          0x40000000, 0x800, 0);
    new_var = pmf_Presenter_StateWin;
    *new_var2 = new_var;
}
}
}

namespace s020c26f4 {
extern "C" {
void Presenter_StateWin(char *t) {
    int r = Presenter_IsAnimDone(t);
    if (r == 0) return;
    Presenter_PlayIdle(t);
}
}
}

namespace s020c2664 {
extern "C" {
extern int sPresenterMissTexAnim[];
extern int pmf_Presenter_StateMistake[];

struct W2 { int w[2]; };

void Presenter_PlayMiss(char *c) {
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        c + 8, ((void *)PRESENTER(c)->mAnims[1].file), 4, 0x40000000, 0x800, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        c + 0xc8, ((void *)PRESENTER(c)->mTexSeqs[1].file), 0x40000000, 0x800, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0xdc, sPresenterMissTexAnim,
                                                          0x40000000, 0x800, 0);
    *(struct W2 *)c = *(struct W2 *)pmf_Presenter_StateMistake;
}
}
}

namespace s020c263c {
extern "C" {
void Presenter_StateMistake(char *t) {
    int r = Presenter_IsAnimDone(t);
    if (r == 0) return;
    Presenter_PlayIdle(t);
}
}
}

namespace s020c2594 {
extern "C" {
extern "C" {
extern int sPresenterCheerTexAnim;
}
extern "C" {
extern volatile long long pmf_Presenter_StateSurprisedThenSmile;
}
void Presenter_PlayCheer(void *c) {
    char *r4 = (char *)c;
    if ((PRESENTER(r4)->mQuiet) == 0) {
        func_02012174(0, 3);
    }
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        r4 + 8, ((void *)PRESENTER(r4)->mAnims[7].file), 8, 0x40000000, 0x800, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        r4 + 0xc8, ((void *)PRESENTER(r4)->mTexSeqs[7].file), 0x40000000, 0x800, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &sPresenterCheerTexAnim,
                                                          0x40000000, 0x800, 0);
    *((long long *)r4) = pmf_Presenter_StateSurprisedThenSmile;
}
}
}

namespace s020c24e4 {
extern "C" {
extern "C" {
extern int sPresenterSmileTexAnim;
}
void Presenter_StateSurprisedThenSmile(void *c) {
    char *r4 = (char *)c;
    if (!Presenter_IsAnimDone(r4))
        return;
    if (PRESENTER(r4)->mFaceAnim != PRESENTER(r4)->mAnims[7].file)
        return;
    if (PRESENTER(r4)->mQuiet == 0)
        _ZN5Sound12PlayBank2_2DEj(0x1d1);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        r4 + 8, ((void *)PRESENTER(r4)->mAnims[6].file), 8, 0, 0x800, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        r4 + 0xc8, ((void *)PRESENTER(r4)->mTexSeqs[6].file), 0, 0x800, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &sPresenterSmileTexAnim, 0,
                                                          0x800, 0);
}
}
}

namespace s020c2440 {
extern "C" {
struct G2 { int w[2]; };
extern "C" {
extern struct G2 pmf_Presenter_StateLoseStart;
}
extern "C" {
extern void *sPresenterLoseTexAnim;
}
void Presenter_PlayLose(char *c) {
    if (PRESENTER(c)->mQuiet == 0)
        _ZN5Sound12PlayBank2_2DEj(0x1c9);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        c + 8, ((void *)PRESENTER(c)->mAnims[4].file), 0, 0x40000000, 0x800, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        c + 0xc8, ((void *)PRESENTER(c)->mTexSeqs[4].file), 0x40000000, 0x800, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0xdc, (void *)&sPresenterLoseTexAnim,
                                                          0x40000000, 0x800, 0);
    *(struct G2 *)c = pmf_Presenter_StateLoseStart;
}
}
}

namespace s020c23a8 {
extern "C" {
extern "C" {
extern int sPresenterLoseLoopTexAnim;
}
void Presenter_StateLoseStart(void *c) {
    char *r4 = (char *)c;
    if (!Presenter_IsAnimDone(r4))
        return;
    if (PRESENTER(r4)->mFaceAnim != PRESENTER(r4)->mAnims[4].file)
        return;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        r4 + 8, ((void *)PRESENTER(r4)->mAnims[3].file), 0, 0, 0x800, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        r4 + 0xc8, ((void *)PRESENTER(r4)->mTexSeqs[3].file), 0, 0x800, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &sPresenterLoseLoopTexAnim, 0,
                                                          0x800, 0);
}
}
}

namespace s020c2300 {
extern "C" {
extern "C" {
extern int sPresenterCheerTexAnim;
}
extern "C" {
extern long long pmf_Presenter_StateSurprisedThenIdle;
}
void Presenter_PlayCheerLoop(void *c) {
    long long new_var;
    char *r4 = (char *)c;
    if ((*((int *)(((char *)c) + 0x1a8))) == 0) {
        func_02012174(0, 3);
    }
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        r4 + 8, ((void *)PRESENTER(r4)->mAnims[7].file), 8, 0x40000000, 0x800, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        r4 + 0xc8, ((void *)PRESENTER(r4)->mTexSeqs[7].file), 0x40000000, 0x800, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &sPresenterCheerTexAnim,
                                                          0x40000000, 0x800, 0);
    *((long long *)r4) = (new_var = pmf_Presenter_StateSurprisedThenIdle & 0xFFFFFFFFFFFFFFFF);
}
}
}

namespace s020c22d8 {
extern "C" {
void Presenter_StateSurprisedThenIdle(char *t) {
    int r = Presenter_IsAnimDone(t);
    if (r == 0) return;
    Presenter_PlayIdle(t);
}
}
}

namespace s020c2290 {
extern "C" {
// @symbol Presenter_UpdateMatrix
extern "C" { extern struct Matrix4x3 data_020a0e68; }
void Presenter_UpdateMatrix(char* c) {
    Matrix4x3_FromTranslation(&data_020a0e68, PRESENTER(c)->mPos.x, PRESENTER(c)->mPos.y,
                              PRESENTER(c)->mPos.z);
    *(struct Matrix4x3*)(c+0x24) = data_020a0e68;
}
}
}

namespace s020c225c {
extern "C" {
// @symbol Camera_InitPresenterView
/* Camera_InitPresenterView at 0x020c225c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */

struct Matrix4x3_local { int data[12]; };

struct dCamera_c {
    struct Matrix4x3_local viewMat;  /* 0x00 */
    char pad30[0x30];          /* 0x30 */
    struct Matrix4x3_local projMat;  /* 0x60 */
    char pad90[0x10];          /* 0x90 */
    struct Vector3 eye;        /* 0xa0 */
    struct Vector3 target;     /* 0xac */
    short angle;               /* 0xb8 */
};


void Camera_InitPresenterView(struct dCamera_c *self)
{
    self->eye.x = 0;
    self->eye.y = 0;
    self->eye.z = 0;
    self->target.x = 0;
    self->target.y = 0;
    self->target.z = 0x2d000;
    self->angle = 0x800;
    Camera_UpdateMatrices(self);
}
}
}

namespace s020c221c {
extern "C" {
int Mascot_Construct(char *t)
{
    func_02017acc(t, 0x205);
    _ZN13SharedFilePtr9ConstructEj(t + 8, 0x206);
    _ZN14BlendModelAnimC1Ev(t + 0x10);
    MASCOT(t)->mLooking = 0;
    return (int)t;
}
}
}

namespace s020c21e4 {
extern "C" {
int Mascot_Destruct(char *t)
{
    _ZN13SharedFilePtr7ReleaseEv(t);
    _ZN13SharedFilePtr7ReleaseEv(t + 8);
    _ZN14BlendModelAnimD1Ev(t + 0x10);
    SharedFilePtr_Destruct_Anim(t + 8);
    func_02017ab4(t);
    return (int)t;
}
}
}

namespace s020c2154 {
extern "C" {
void Mascot_Load(char *c)
{
    void *f;
    f = _ZN5Model8LoadFileER13SharedFilePtr(c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x10, f, 1, -1);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 8);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        c + 0x10, ((void *)MASCOT(c)->mWaitAnimFile[1]), 0, 0, 0x800, 0);
    _ZN5Model12HideMaterialEii(c + 0x10, 7, 0);
    MASCOT(c)->mPos.x = 0xd000;
    MASCOT(c)->mPos.y = 0;
    MASCOT(c)->mPos.z = 0;
    MASCOT(c)->mRot[0] = 0;
    MASCOT(c)->mRot[1] = -0x1400;
    MASCOT(c)->mRot[2] = 0;
}
}
}

namespace s020c2144 {
extern "C" {
/* Mascot_Update @ 0x20c2144 (ov006) -- veneer: add r0,r0,#0x10; b BlendModelAnim::Advance(). */

void Mascot_Update(void* a) {
    _ZN14BlendModelAnim7AdvanceEv((char*)a + 0x10);
}
}
}

namespace s020c201c {
typedef short s16;

/* Mascot.h's layout, with its BlendModelAnim spelled as the local Model so
   Draw can call Render on it. */
struct C {
    char pad10[0x10];
    Model model;
    char pad[0x80 - 0x14];
    Vector3 mLookTarget;
    Vector3 mPos;
    s16 mRot[3];
    char pada2[0xa2 - 0x9e];
    s16 mHeadPitch;
    s16 mHeadYaw;
    char pada8[0xa8 - 0xa6];
    int mLooking;
};


extern "C" void Mascot_Draw(C *c)
{
    if (c->mLooking != 0) {
        s16 h = Vec3_HorzAngle(&c->mPos, &c->mLookTarget);
        s16 v = Vec3_VertAngle(&c->mPos, &c->mLookTarget);
        s16 dh = (s16)(h - c->mRot[1]);
        if (dh < -0x2800) dh = -0x2800;
        else if (dh > 0x2800) dh = 0x2800;
        if (v < -0x1000) v = -0x1000;
        else if (v > 0x1000) v = 0x1000;
        UpdateAngle(c->mHeadYaw, dh, 8, 0x200);
        UpdateAngle(c->mHeadPitch, v, 8, 0x200);
        dh = (s16)(h - dh);
        UpdateAngle(c->mRot[1], dh, 8, 0x200);
    } else {
        UpdateAngle(c->mHeadPitch, 0, 8, 0x200);
        UpdateAngle(c->mHeadYaw, 0x800, 8, 0x200);
        UpdateAngle(c->mRot[1], -0x1400, 8, 0x200);
    }
    Mascot_UpdateMatrix((char *)c);
    c->model.Render(0);
}
}

namespace s020c1f4c {
// @symbol Mascot_UpdateMatrix
typedef short s16;

extern "C" {
extern "C" { extern Matrix4x3 data_020a0e68; }
}

struct Vtbl {
  virtual void v0();
  virtual void v1();
  virtual void v2();
  virtual void v3();   // offset 0xc
};

struct Sub { char pad[0x120]; Matrix4x3 m; };

extern "C" void Mascot_UpdateMatrix(char* c){
    Matrix4x3_FromTranslation(&data_020a0e68, MASCOT(c)->mPos.x, MASCOT(c)->mPos.y,
                              MASCOT(c)->mPos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, MASCOT(c)->mRot[1]);
    *(Matrix4x3 *)(c + 0x2c) = data_020a0e68;
    ((Vtbl *)(c + 0x10))->v3();
    data_020a0e68 = (*(Sub **)(c + 0x24))->m;
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, MASCOT(c)->mHeadYaw);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, MASCOT(c)->mHeadPitch);
    (*(Sub **)(c + 0x24))->m = data_020a0e68;
}
}

namespace s020c1f04 {
extern "C" {
void Mascot_LookAt(char* c, int* src) {
    if (src != 0) {
        MASCOT(c)->mLookTarget.x = src[0];
        MASCOT(c)->mLookTarget.y = src[1];
        MASCOT(c)->mLookTarget.z = src[2];
    } else {
        MASCOT(c)->mLookTarget.x = 0;
        MASCOT(c)->mLookTarget.y = 0x2000;
        MASCOT(c)->mLookTarget.z = -0x2000;
    }
    MASCOT(c)->mLooking = 1;
}
}
}

namespace s020c1ef8 {
extern "C" {
void Mascot_StopLooking(int *p)
{
    MASCOT(p)->mLooking = 0;
}
}
}

namespace s020c1eb4 {
extern "C" {
// @symbol Camera_InitSlotsView
/* Camera_InitSlotsView at 0x020c1eb4
 *
 * dCamera_c preset init: sets eye/target vectors and angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of Camera_InitPresenterView.
 */

struct Matrix4x3_local { int data[12]; };

struct dCamera_c {
    struct Matrix4x3_local viewMat;  /* 0x00 */
    char pad30[0x30];          /* 0x30 */
    struct Matrix4x3_local projMat;  /* 0x60 */
    char pad90[0x10];          /* 0x90 */
    struct Vector3 eye;        /* 0xa0 */
    struct Vector3 target;     /* 0xac */
    short angle;               /* 0xb8 */
};


void Camera_InitSlotsView(struct dCamera_c *self)
{
    self->eye.x = 0;
    self->eye.y = 0xe000;
    self->eye.z = -0x22f00;
    self->target.x = 0;
    self->target.y = 0xf000;
    self->target.z = 0x32000;
    self->angle = 0xb00;
    Camera_UpdateMatrices(self);
}
}
}

namespace s020c1d80 {
extern "C" {
int Dealer_Construct(char *t)
{
    DEALER(t)->mHand = 0;
    _ZN14BlendModelAnimC1Ev(t + 0x1c);
    _ZN5ModelC1Ev(t + 0x8c);
    MascotWalker_Construct(t + 0xdc);
    DEALER(t)->mShuffleRound = 0;
    DEALER(t)->mReserved = 0;
    func_02017acc(t + 0x1e8, 0x208);
    func_02017acc(t + 0x1f0, 0);
    _ZN13SharedFilePtr9ConstructEj(t + 0x1f8, 0x212);
    _ZN13SharedFilePtr9ConstructEj(t + 0x200, 0x210);
    _ZN13SharedFilePtr9ConstructEj(t + 0x208, 0x20f);
    _ZN13SharedFilePtr9ConstructEj(t + 0x210, 0x20c);
    _ZN13SharedFilePtr9ConstructEj(t + 0x218, 0x209);
    _ZN13SharedFilePtr9ConstructEj(t + 0x220, 0x20a);
    _ZN13SharedFilePtr9ConstructEj(t + 0x228, 0x20b);
    _ZN13SharedFilePtr9ConstructEj(t + 0x230, 0x20d);
    _ZN13SharedFilePtr9ConstructEj(t + 0x238, 0x211);
    _ZN13SharedFilePtr9ConstructEj(t + 0x240, 0x215);
    _ZN13SharedFilePtr9ConstructEj(t + 0x248, 0x213);
    _ZN13SharedFilePtr9ConstructEj(t + 0x250, 0x214);
    _ZN13SharedFilePtr9ConstructEj(t + 0x258, 0x20e);
    DEALER(t)->mModelData = 0;
    DEALER(t)->mTableData = 0;
    DEALER(t)->mShuffleSoundPlayed = 0;
    DEALER(t)->mDealSoundPlayed = 0;
    return (int)t;
}
}
}

namespace s020c1c64 {
extern "C" {
int Dealer_Destruct(char *t)
{
    _ZN13SharedFilePtr7ReleaseEv(t + 0x1e8);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x1f0);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x1f8);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x200);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x208);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x210);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x218);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x220);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x228);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x230);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x238);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x240);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x248);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x250);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x258);
    SharedFilePtr_Destruct_Anim(t + 0x258);
    SharedFilePtr_Destruct_Anim(t + 0x250);
    SharedFilePtr_Destruct_Anim(t + 0x248);
    SharedFilePtr_Destruct_Anim(t + 0x240);
    SharedFilePtr_Destruct_Anim(t + 0x238);
    SharedFilePtr_Destruct_Anim(t + 0x230);
    SharedFilePtr_Destruct_Anim(t + 0x228);
    SharedFilePtr_Destruct_Anim(t + 0x220);
    SharedFilePtr_Destruct_Anim(t + 0x218);
    SharedFilePtr_Destruct_Anim(t + 0x210);
    SharedFilePtr_Destruct_Anim(t + 0x208);
    SharedFilePtr_Destruct_Anim(t + 0x200);
    SharedFilePtr_Destruct_Anim(t + 0x1f8);
    func_02017ab4(t + 0x1f0);
    func_02017ab4(t + 0x1e8);
    MascotWalker_Destruct(t + 0xdc);
    _ZN5ModelD1Ev(t + 0x8c);
    _ZN14BlendModelAnimD1Ev(t + 0x1c);
    return (int)t;
}
}
}

namespace s020c1a88 {
extern "C" {
// @symbol Dealer_Load
extern "C" { extern struct Vector3 data_020a0ebc; }
extern "C" { extern struct Vector3_16 data_020a0edc; }


int Dealer_Load(char *c)
{
    void *f;
    void *ip;

    _ZN17dExtShadowModel_c8CleanAllEv();
    *(void **)(c + 0x260) = _ZN5Model8LoadFileER13SharedFilePtr(c + 0x1e8);
    *(void **)(c + 0x264) = _ZN5Model8LoadFileER13SharedFilePtr(c + 0x1f0);
    f = ((void *)DEALER(c)->mModelData);
    if (f == 0 || _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x1c, f, 1, -1) == 0) {
        return 0;
    }
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x1f8);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x200);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x208);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x210);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x218);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x220);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x228);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x230);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x238);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x240);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x248);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x250);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 0x258);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        c + 0x1c, ((void *)DEALER(c)->mAnims[0].file), 0, 0, 0x800, 0);
    Dealer_ResetMatrices(c);
    *(int *)(c + 8) = data_020a0ebc.x;
    DEALER(c)->mPos.y = data_020a0ebc.y;
    DEALER(c)->mPos.z = data_020a0ebc.z;
    DEALER(c)->mRot[0] = data_020a0edc.x;
    DEALER(c)->mRot[1] = data_020a0edc.y;
    DEALER(c)->mRot[2] = data_020a0edc.z;
    DEALER(c)->mDealtCount = 0;
    _ZN5Model12HideMaterialEii(c + 0x1c, 1, 0);
    Dealer_PlayIdle(c);
    f = ((void *)DEALER(c)->mTableData);
    if (f == 0 || _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x8c, f, 1, -1) == 0) {
        return 0;
    }
    ip = *(void **)(c + 0x98);
    *(int *)((char *)ip + 0x24) &= ~1;
    *(int *)((char *)ip + 0x114) &= ~1;
    *(int *)((char *)ip + 0xe4) |= 2;
    MascotWalker_Load(c + 0xdc);
    DEALER(c)->mLampTimer = 0;
    DEALER(c)->mLampPattern = 1;
    DEALER(c)->mLightAngle = 0;
    return 1;
}
}
}

namespace s020c19d0 {
struct C;
typedef void (C::*PMF)();
struct C {
    PMF pmf;
};
extern "C" { extern short data_ov006_0212b89c[]; }

extern "C" void Dealer_Update(char* thiz)
{
    char* c = thiz;
    C* o = (C*)c;
    if (*(int*)c != 0) {
        (o->*(o->pmf))();
    }
    _ZN14BlendModelAnim7AdvanceEv(c + 0x1c);
    if (_Z15ApproachLinear2Rsss((short*)(c + 0x1e0), 0, 1) != 0) {
        short* p = (short*)(((int)c + 0x1e2));
        *p = *p + 1;
        if (*(short*)(c + 0x100 + 0xe2) > 7)
            *(short*)(c + 0x100 + 0xe2) = 1;
        *(short*)(c + 0x1e0) = data_ov006_0212b89c[*(short*)(c + 0x100 + 0xe2)];
    }
    short* q = (short*)(((int)c + 0x1e4));
    *q = *q + 0x400;
    MascotWalker_Update(c + 0xdc);
}
}

namespace s020c1804 {
// @symbol Dealer_Draw

extern "C" {
}


struct Obj {
    virtual void f0() = 0;
    virtual void f1() = 0;
    virtual void f2() = 0;
    virtual void f3() = 0;
    virtual void f4() = 0;
    virtual void f5(int a) = 0;
};

extern "C" void Dealer_Draw(void *self)
{
    char *c = (char*)self;
    struct Vector3 v1, v2;
    unsigned int p1, p2;
    short ang;
    unsigned char b[3];
    int i;

    v1.x = 0;
    v1.y = 0;
    v1.z = 0xfffff008;
    v2.x = 0;
    v2.y = 0;
    v2.z = 0xfffff008;
    func_0203cd80(&v1, -0x1000);
    func_0203ccd4(&v1, 0x1000);

    ang = DEALER(c)->mLightAngle;
    func_0203ccd4(&v2, ang);

    p1 = (((short)v1.x >> 3) & 0x3ff)
       | ((((short)v1.y >> 3) & 0x3ff) << 10)
       | ((((short)v1.z >> 3) & 0x3ff) << 20);
    *(volatile unsigned int*)0x40004c8 = p1;

    p2 = (((short)v2.x >> 3) & 0x3ff)
       | ((((short)v2.y >> 3) & 0x3ff) << 10)
       | ((((short)v2.z >> 3) & 0x3ff) << 20);
    *(volatile unsigned int*)0x40004c8 = p2 | 0x40000000;

    func_02016a14(c + 0x1c, 0x7fff);
    func_02016a04(c + 0x1c, 0x4210);

    ang = DEALER(c)->mLampPattern;
    {
        unsigned char *p = b;
        for (i = 0; i < 3; i++) {
            if ((ang >> i) & 1)
                *p = 0x1f;
            else
                *p = 0x14;
            p++;
        }
    }

    func_020169d8(c + 0x8c, 0, (unsigned short)(b[0] | (b[1] << 5) | (b[2] << 10)));
    func_020169d8(c + 0x8c, 4, 0x6318);
    func_020169d8(c + 0x8c, 5, (unsigned short)(b[1] | (b[2] << 5) | (b[0] << 10)));

    ((Obj*)(c + 0x1c))->f5(0);
    MascotWalker_Draw(c + 0xdc);
    ((Obj*)(c + 0x8c))->f5(0);

    dExtShadowModel_c::RenderAll();
}
}

namespace s020c1764 {
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim {
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */

struct P2 { int w[2]; };

extern "C" { extern P2 pmf_Dealer_StateIdle; }

extern "C" void Dealer_PlayIdle(char *c)
{
    if (DEALER(c)->mHand == 1) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            (BlendModelAnim *)(c + 0x1c), ((void *)DEALER(c)->mAnims[7].file), 0, 0, 0x800, 0);
        if (DEALER(c)->mDealSoundPlayed == 0) {
            Sound::PlayBank2_2D(0x13a);
            DEALER(c)->mDealSoundPlayed = 1;
        }
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            (BlendModelAnim *)(c + 0x1c), ((void *)DEALER(c)->mAnims[0].file), 0, 0, 0x800, 0);
    }
    *(P2 *)c = pmf_Dealer_StateIdle;
}
}

namespace s020c1760 {
extern "C" {
void Dealer_StateIdle(void)
{
}
}
}

namespace s020c1718 {
extern "C" {
extern int pmf_Dealer_StateIdle_2[];
int Dealer_IsIdle(int* r0) {
    int* g = pmf_Dealer_StateIdle_2;
    int w0 = r0[0];
    int ip = 1;
    if (w0 == g[0]) {
        if (r0[1] == g[1] || w0 == 0) ip = 0;
    }
    return ip == 0;
}
}
}

namespace s020c16b4 {
extern "C" {
extern int pmf_Dealer_StateIdle_3[];
int Dealer_IsIdleAtRest(char *c)
{
    int *G = pmf_Dealer_StateIdle_3;
    int r3 = *(int *)c;
    int r4 = 0;
    int lr = 1;
    if (r3 == G[0] && (*(int *)(c + 4) == G[1] || r3 == 0))
        lr = 0;
    if (lr == 0 && DEALER(c)->mAnim == DEALER(c)->mAnims[0].file)
        r4 = 1;
    return r4;
}
}
}

namespace s020c1604 {
extern "C" {
typedef int Fix12;
typedef short s16;
struct M8 { int w[2]; };
extern char pmf_Dealer_StateShuffle[];

void Dealer_StartShuffle(char *c, int unused, short a2, int a3) {
    if (DEALER(c)->mShuffleRound == 0) {
        DEALER(c)->mShuffleLeft = 4;
        DEALER(c)->mDealDoneAt = 3;
    } else {
        DEALER(c)->mShuffleLeft = 1;
        DEALER(c)->mDealDoneAt = 1;
    }
    if (_Z15ApproachLinear2Rsss((short *)(c + 0x1dc), 5, 1) != 0) {
        DEALER(c)->mShuffleRound = 0;
    }
    DEALER(c)->mDealSoundPlayed = 0;
    DEALER(c)->mDealLeft = a2;
    DEALER(c)->mDealtCount = a3;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        c + 0x1c, ((void *)DEALER(c)->mAnims[1].file), 0, 0x40000000, 0x800, 0);
    *(struct M8 *)c = *(struct M8 *)pmf_Dealer_StateShuffle;
}
}
}

namespace s020c14bc {
struct V {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21();
};


/* local extern: only slot 21 is called, through the view V above. */
extern "C" { extern V* gMgScene; }

extern "C" void Dealer_StateShuffle(char *c) {
    if (DEALER(c)->mShuffleSoundPlayed == 0) {
        _ZN5Sound12PlayBank2_2DEj(0x139);
        DEALER(c)->mShuffleSoundPlayed = 1;
    }

    if (DEALER(c)->mAnim == DEALER(c)->mAnims[1].file) {
        if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x6c))
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                c + 0x1c, ((void *)DEALER(c)->mAnims[2].file), 0, 0, 0x800, 0);
        return;
    }

    if (_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 0)) {
        short cv = *(short*)(c + 0x100 + 0xd8);
        short* p = (short*)(((long)c + 0x1d8));
        short old = *p;
        *p = old - 1;
        if (cv <= 0) {
            Dealer_StartDeal(c, DEALER(c)->mDealLeft, DEALER(c)->mDealtCount);
            return;
        }
    }

    if (!_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 4))
        return;

    _ZN5Sound12PlayBank2_2DEj(0x141);
    if (DEALER(c)->mShuffleLeft == DEALER(c)->mDealDoneAt) {
        V* o = gMgScene;
        if (o)
            o->v21();
    }
}
}

namespace s020c1420 {
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim {
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */

struct P2 { int w[2]; };

extern "C" {
extern P2 pmf_Dealer_StateDeal;
}

extern "C" void Dealer_StartDeal(char *c, short arg1, int arg2) {
    DEALER(c)->mDealLeft = arg1;
    DEALER(c)->mDealtCount = arg2;
    if (Dealer_IsIdle((int *)c) && DEALER(c)->mAnim != DEALER(c)->mAnims[7].file) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c),
                                                           ((void *)DEALER(c)->mAnims[1].file), 0,
                                                           0x40000000, 0x800, 0);
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c),
                                                           ((void *)DEALER(c)->mAnims[3].file), 0,
                                                           0x40000000, 0x800, 0);
    }
    *(P2 *)c = pmf_Dealer_StateDeal;
}
}

namespace s020c11c0 {
extern "C" {

void Dealer_StateDeal(char *c) {
    void *f7c = ((void *)DEALER(c)->mAnim);

    if (f7c == ((void *)DEALER(c)->mAnims[3].file)) {
        if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x6c)) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                c + 0x1c, ((void *)DEALER(c)->mAnims[4].file), 0, 0, 0x800, 0);
        }
    }

    f7c = ((void *)DEALER(c)->mAnim);
    if (f7c == ((void *)DEALER(c)->mAnims[5].file) || f7c == ((void *)DEALER(c)->mAnims[6].file)) {
        if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x6c)) {
            DEALER(c)->mDealtCount = 0;
            Dealer_PlayIdle(c);
        }
        return;
    }

    if (f7c == ((void *)DEALER(c)->mAnims[4].file)) {
        if (_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 0)) {
            short cv = *(short *)(c + 0x100 + 0xde);
            short *p = (short *)(((long)c + 0x1de));
            short old = *p;
            *p = old - 1;
            if (cv <= 0) {
                short state = DEALER(c)->mHand;
                if (state == 0) {
                    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                        c + 0x1c, ((void *)DEALER(c)->mAnims[5].file), 0, 0x40000000, 0x800, 0);
                    if (DEALER(c)->mDealSoundPlayed != 0)
                        return;
                    _ZN5Sound12PlayBank2_2DEj(0x13a);
                    DEALER(c)->mDealSoundPlayed = 1;
                    return;
                } else if (state == 2) {
                    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                        c + 0x1c, ((void *)DEALER(c)->mAnims[6].file), 0, 0x40000000, 0x800, 0);
                    if (DEALER(c)->mDealSoundPlayed != 0)
                        return;
                    _ZN5Sound12PlayBank2_2DEj(0x13a);
                    DEALER(c)->mDealSoundPlayed = 1;
                    return;
                } else {
                    Dealer_PlayIdle(c);
                    return;
                }
            }
        }

        if (DEALER(c)->mDealtCount == 0)
            return;
        if (!_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 5)) return;
        _ZN5Sound12PlayBank2_2DEj(0x142);
        {
            short *p = *(short **)(c + 0x1d4);
            *p = (short)(*p + 1);
        }
        return;
    }

    if (f7c == ((void *)DEALER(c)->mAnims[1].file)) {
        if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x6c)) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                c + 0x1c, ((void *)DEALER(c)->mAnims[3].file), 0, 0x40000000, 0x800, 0);
        }
    }
}
}
}

namespace s020c1164 {
extern "C" {
struct P2 { int w[2]; };
extern "C" { extern P2 pmf_Dealer_StateReveal_2; }

void Dealer_StartReveal(int *t, int a1, int a2)
{
    *(short *)((char *)t + 0x1de) = a1;
    t[0x75] = a2;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x1c, (void *)t[0x87], 8, 0, 0x800, 0);
    *(P2 *)t = pmf_Dealer_StateReveal_2;
}
}
}

namespace s020c0f9c {
extern "C" {
}

extern "C" void Dealer_StateReveal(void *cc)
{
    char *c = (char*)cc;

    if (DEALER(c)->mAnim != DEALER(c)->mAnims[4].file)
        goto other;

    if (_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 0)) {
        short cv = *(short*)(c + 0x100 + 0xde);
        short *p = (short*)(((long)c + 0x1de));
        short old = *p;
        *p = old - 1;
        if (cv <= 0) {
            int a;
            DEALER(c)->mDealtCount = 0;
            a = DEALER(c)->mHand;
            if (a == 0) {
                _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                    c + 0x1c, ((void *)DEALER(c)->mAnims[5].file), 0, 0x40000000, 0x800, 0);
                if (DEALER(c)->mDealSoundPlayed != 0)
                    return;
                _ZN5Sound12PlayBank2_2DEj(0x13a);
                DEALER(c)->mDealSoundPlayed = 1;
                return;
            } else if (a == 2) {
                _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                    c + 0x1c, ((void *)DEALER(c)->mAnims[6].file), 0, 0x40000000, 0x800, 0);
                if (DEALER(c)->mDealSoundPlayed != 0)
                    return;
                _ZN5Sound12PlayBank2_2DEj(0x13a);
                DEALER(c)->mDealSoundPlayed = 1;
                return;
            } else {
                Dealer_PlayIdle(c);
                return;
            }
        }
    }

    if (DEALER(c)->mDealtCount == 0)
        return;
    if (!_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 5))
        return;
    _ZN5Sound12PlayBank2_2DEj(0x142);
    *(short*)*(char**)(c + 0x1d4) += 1;
    return;

other:
    if (DEALER(c)->mAnim == DEALER(c)->mAnims[5].file ||
        DEALER(c)->mAnim == DEALER(c)->mAnims[6].file) {
        if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x6c))
            Dealer_PlayIdle(c);
    }
}
}

namespace s020c0f0c {
extern "C" {
extern "C" {
extern int pmf_Dealer_StateDeal_2[2];
}
extern "C" { extern int pmf_Dealer_StateReveal[2]; }

int Dealer_IsBusy(int *c)
{
    int *p = pmf_Dealer_StateDeal_2;
    int r = 1;
    int m1 = r;
    int x = *c;
    if (x == *p) {
        if (c[1] == p[1] || x == 0)
            m1 = 0;
    }
    if (m1 != 0) {
        int m2 = 1;
        p = pmf_Dealer_StateReveal;
        if (x == *p) {
            if (c[1] == p[1] || x == 0)
                m2 = 0;
        }
        if (m2)
            r = 0;
    }
    return r;
}
}
}

namespace s020c0efc {
extern "C" {
/* Dealer_IsAnimDone @ 0x20c0efc (ov006) -- veneer: add r0,r0,#0x6c; b dExtFrameCtrl_c::Finished(). */

int Dealer_IsAnimDone(void* a) {
    return _ZN15dExtFrameCtrl_c8FinishedEv((char*)a + 0x6c);
}
}
}

namespace s020c0e8c {
extern "C" {
struct P2 { int w[2]; };
extern "C" { extern P2 pmf_Dealer_StateSwap; }

void Dealer_StartSwap(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x1c, (void *)t[0x8f], 0, 0x40000000, 0x800, 0);
    _ZN5Model12HideMaterialEii((char *)t + 0x1c, 2, 0);
    _ZN5Model12ShowMaterialEii((char *)t + 0x1c, 1, 0);
    *(P2 *)t = pmf_Dealer_StateSwap;
}
}
}

namespace s020c0df0 {

extern "C" void Dealer_StateSwap(char *c)
{
    if (DEALER(c)->mShuffleSoundPlayed == 0) {
        Sound::PlayBank2_2D(0x139);
        DEALER(c)->mShuffleSoundPlayed = 1;
    }
    if (DEALER(c)->mAnim == DEALER(c)->mAnims[8].file &&
        ((dExtFrameCtrl_c *)(c + 0x6c))->Finished()) {
        ((Model *)(c + 0x1c))->HideMaterial(1, 0);
        ((Model *)(c + 0x1c))->ShowMaterial(2, 0);
        return;
    }
    if (((dExtFrameCtrl_c *)(c + 0x6c))->WillHitFrame(0x14))
        Sound::PlayBank2_2D(0x156);
}
}

namespace s020c0d68 {
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim { int pad; void SetAnim(BCA_File &f, int a, int b, Fix12 d, unsigned short e); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */

extern "C" { extern double pmf_Dealer_StateLose; }

extern "C" void Dealer_PlayLose(char *c)
{
    if (DEALER(c)->mHand == 1) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c),
                                                           (void *)DEALER(c)->mAnims[5].file, 0,
                                                           0x40000000, 0x800, 0);
    } else {
        Sound::PlayBank2_2D(0x13c);
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c),
                                                           (void *)DEALER(c)->mAnims[12].file, 0,
                                                           0x40000000, 0x800, 0);
    }
    *(double *)c = pmf_Dealer_StateLose;
}
}

namespace s020c0ce8 {
extern "C" {
typedef int Fix12;
void Dealer_StateLose(char *c){
    if(!_ZN15dExtFrameCtrl_c8FinishedEv(c+0x6c)) return;
    if (DEALER(c)->mAnim == DEALER(c)->mAnims[12].file) {
        Dealer_PlayIdle(c);
        return;
    }
    _ZN5Sound12PlayBank2_2DEj(0x13c);
    DEALER(c)->mHand = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        c + 0x1c, ((void *)DEALER(c)->mAnims[12].file), 0, 0x40000000, 0x800, 0);
}
}
}

namespace s020c0c80 {
extern "C" {
struct G2 { int w[2]; };
extern "C" { extern struct G2 pmf_Dealer_StateWin; }
void Dealer_PlayWin(void *c)
{
    char *r4 = (char *)c;
    _ZN5Sound12PlayBank2_2DEj(0x13b);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        r4 + 0x1c, ((void *)DEALER(r4)->mAnims[9].file), 0, 0x40000000, 0x800, 0);
    DEALER(r4)->mShuffleLeft = 5;
    *(struct G2 *)r4 = pmf_Dealer_StateWin;
}
}
}

namespace s020c0b74 {
extern "C" {
typedef struct {
    char _pad[0x1d8];
    short field_1d8;
} Obj;

void Dealer_StateWin(char *p) {
    if (((void *)DEALER(p)->mAnim) == ((void *)DEALER(p)->mAnims[9].file) &&
        _ZN15dExtFrameCtrl_c8FinishedEv(p + 0x6c)) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
            p + 0x1c, ((void *)DEALER(p)->mAnims[10].file), 0, 0, 0x800, 0);
        return;
    }
    if (((void *)DEALER(p)->mAnim) == ((void *)DEALER(p)->mAnims[10].file) &&
        _ZNK15dExtFrameCtrl_c12WillHitFrameEi(p + 0x6c, 0)) {
        DEALER(p)->mShuffleLeft -= 1;
        if (((Obj *)p)->field_1d8 == 0) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
                p + 0x1c, ((void *)DEALER(p)->mAnims[11].file), 0, 0x40000000, 0x800, 0);
            return;
        }
    }
    if (((void *)DEALER(p)->mAnim) == ((void *)DEALER(p)->mAnims[11].file) &&
        _ZN15dExtFrameCtrl_c8FinishedEv(p + 0x6c)) {
        Dealer_PlayIdle(p);
    }
}
}
}

namespace s020c0af8 {
extern "C" {
extern "C" {
extern volatile struct Matrix4x3 sDealerIdentityMtx;
}
// @symbol Dealer_ResetMatrices
void Dealer_ResetMatrices(char* c)
{
    struct Matrix4x3 tmp;
    tmp = *(struct Matrix4x3 *)&sDealerIdentityMtx;
    *(struct Matrix4x3 *)(c + 0x38) = (struct Matrix4x3 &)sDealerIdentityMtx;
    *(struct Matrix4x3*)(c + 0xa8) = tmp;
}
}
}

namespace s020c0aa8 {
extern "C" {
// @symbol Camera_InitDealerView
/* Camera_InitDealerView at 0x020c0aa8
 *
 * dCamera_c preset init: sets eye/target vectors and angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of Camera_InitPresenterView.
 */

struct Matrix4x3_local { int data[12]; };

struct dCamera_c {
    struct Matrix4x3_local viewMat;  /* 0x00 */
    char pad30[0x30];          /* 0x30 */
    struct Matrix4x3_local projMat;  /* 0x60 */
    char pad90[0x10];          /* 0x90 */
    struct Vector3 eye;        /* 0xa0 */
    struct Vector3 target;     /* 0xac */
    short angle;               /* 0xb8 */
};


void Camera_InitDealerView(struct dCamera_c *self)
{
    self->eye.x = 0x200;
    self->eye.y = 0x9600;
    self->eye.z = -0x22f00;
    self->target.x = -0x100;
    self->target.y = 0x10500;
    self->target.z = 0x1ee00;
    self->angle = 0xbb0;
    Camera_UpdateMatrices(self);
}
}
}

namespace s020c0a48 {
extern "C" {
int MascotWalker_Construct(char *t)
{
    func_02017acc(t, 0x205);
    _ZN13SharedFilePtr9ConstructEj(t + 8, 0x206);
    _ZN13SharedFilePtr9ConstructEj(t + 0x10, 0x207);
    _ZN14BlendModelAnimC1Ev(t + 0x18);
    _ZN17dExtShadowModel_cC1Ev(t + 0x88);
    WALKER(t)->mPathIndex = 0;
    *(short *)(t + 0xf0) = 0;
    *(int *)(t + 0xf4) = 0;
    return (int)t;
}
}
}

namespace s020c09f8 {
extern "C" {
int MascotWalker_Destruct(char *t)
{
    _ZN13SharedFilePtr7ReleaseEv(t);
    _ZN13SharedFilePtr7ReleaseEv(t + 0x10);
    _ZN13SharedFilePtr7ReleaseEv(t + 8);
    _ZN17dExtShadowModel_cD1Ev(t + 0x88);
    _ZN14BlendModelAnimD1Ev(t + 0x18);
    SharedFilePtr_Destruct_Anim(t + 0x10);
    SharedFilePtr_Destruct_Anim(t + 8);
    func_02017ab4(t);
    return (int)t;
}
}
}

namespace s020c092c {
// @symbol MascotWalker_Load
typedef int Fix12i;
struct SharedFilePtr; struct BMD_File; struct BCA_File;
struct ModelBase { int d; };
struct BlendModelAnim { int d; };

extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
    BlendModelAnim*, BCA_File&, int, int, Fix12i, unsigned short);



extern "C" { extern Vector3 data_020a0ebc; }
extern "C" { extern Vector3_16 data_020a0edc; }

extern "C" void MascotWalker_Load(char* thiz)
{
    char* c = thiz;
    {
        void* bmd = _ZN5Model8LoadFileER13SharedFilePtr(c);
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)(c + 0x18), bmd, 1, -1);
    }
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr((c + 0x10));
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr((c + 8));
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        (BlendModelAnim*)(c + 0x18), **(BCA_File**)(c + 0xc), 0, 0, 0x800, 0);
    _ZN5Model12SetPolygonIDEi((Model*)(c + 0x18), 1);
    _ZN17dExtShadowModel_c12InitCylinderEv((void*)(c + 0x88));
    *(int*)(c + 0xbc) = 0x1000;
    *(int*)(c + 0xc0) = 0x1000;
    *(int*)(c + 0xc4) = 0x1000;
    *(int*)(c + 0xc8) = data_020a0ebc.x;
    *(int*)(c + 0xcc) = data_020a0ebc.y;
    *(int*)(c + 0xd0) = data_020a0ebc.z;
    *(short*)(c + 0xe8) = data_020a0edc.x;
    *(short*)(c + 0xea) = data_020a0edc.y;
    *(short*)(c + 0xec) = data_020a0edc.z;
    MascotWalker_StartNextPath(c);
}
}

namespace s020c07e8 {
typedef int s16;
struct Vector3 { int x, y, z; };

struct C;
typedef void (C::*PMF)();

struct C {
    char pad0[0xb4];
    PMF pmf;        // 0xb4 (2 words)
    char pad1[0xc8 - 0xb4 - 8];
    Vector3 v0c8;   // 0xc8
    char pad2[0xea - 0xc8 - 12];
    short a0ea;     // 0xea
    char pad3[0xf0 - 0xea - 2];
    short a0f0;     // 0xf0
    char pad4[0xf4 - 0xf0 - 2];
    int a0f4;       // 0xf4
};

extern "C" {
extern Vector3 sMascotWalkerOffset;
}
extern "C" { extern "C" int pmf_MascotWalker_StateWalk_2[2]; }

extern "C" void MascotWalker_Update(C* c)
{
    (c->*(c->pmf))();

    _ZN14BlendModelAnim7AdvanceEv((char*)c + 0x18);

    if (c->a0f4 != 0) {
        Vector3 t;
        t.x = sMascotWalkerOffset.x;
        t.y = sMascotWalkerOffset.y;
        t.z = sMascotWalkerOffset.z;
        short d;
        int ang;
        ang = Vec3_HorzAngle(&c->v0c8, &t);
        d = (short)(ang - c->a0ea);
        if (d < -0x3000) d = -0x3000;
        else if (d > 0x3000) d = 0x3000;
        _Z11UpdateAngleRssis(&c->a0f0, d, 8, 0x200);

        {
            int* p = (int*)(((int)c + 0xb4));
            int* g = pmf_MascotWalker_StateWalk_2;
            if (p[0] == g[0]) {
                if (p[1] == g[1])
                    return;
                if (*(int*)((char*)c + 0xb4) == 0)
                    return;
            }
        }
        d = (short)(ang - d);
        _Z11UpdateAngleRssis(&c->a0ea, d, 8, 0x200);
    } else {
        _Z11UpdateAngleRssis(&c->a0f0, 0, 8, 0x200);
    }
}
}

namespace s020c07a0 {
extern "C" {
void MascotWalker_Draw(char *t)
{
    MascotWalker_UpdateMatrix(t);
    _ZN17dExtShadowModel_c9InitModelEP9Matrix4x35Fix12IiES3_S3_j(t + 0x88, t + 0x34, 0x3c000, 0x14000, 0x3c000, 0xc);
    _ZN5Model6RenderEPK7Vector3(t + 0x18, 0);
}
}
}

namespace s020c06dc {
// @symbol MascotWalker_UpdateMatrix
typedef short s16;




extern "C" { extern Matrix4x3 data_020a0e68; }

struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void m();  // slot 3 -> offset 0xc
};

extern "C" void MascotWalker_UpdateMatrix(char* thiz)
{
    char* c = thiz;
    ((Base*)(c + 0x18))->m();
    {
        char* m = *(char**)(c + 0x2c);
        data_020a0e68 = *(Matrix4x3*)(m + 0x120);
    }
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(c + 0xf0));
    {
        char* m = *(char**)(c + 0x2c);
        *(Matrix4x3*)(m + 0x120) = data_020a0e68;
    }
    Matrix4x3_FromTranslation((Matrix4x3*)&data_020a0e68, *(int*)(c + 0xc8), *(int*)(c + 0xcc), *(int*)(c + 0xd0));
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(c + 0xea));
    *(Matrix4x3*)(c + 0x34) = data_020a0e68;
}
}

namespace s020c057c {
typedef struct Pair { int a, b; } Pair;
extern "C" {
extern int sMascotWalkerPaths[];
extern int pmf_MascotWalker_StateWalk[];
void MascotWalker_StartNextPath(char* c) {
    int sp8[3];
    int sp14[3];
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x18, (void *)WALKER(c)->mWalkAnim, 0, 0,
                                                       0x800, 0);
    if (WALKER(c)->mPathIndex < 3) {
        *(int *)(((int)c + 0xe4)) += 1;
    } else {
        WALKER(c)->mPathIndex = 0;
    }
    WALKER(c)->mPath = (int)((char *)sMascotWalkerPaths + (WALKER(c)->mPathIndex << 3));
    WALKER(c)->mPathPoint = 0;
    {
        int t = WALKER(c)->mPathPoint * 0xc;
        int *arr = (int *)((int *)WALKER(c)->mPath)[1];
        int* el = (int*)((char*)arr + t);
        WALKER(c)->mPos.x = el[0];
        WALKER(c)->mPos.y = el[1];
        WALKER(c)->mPos.z = el[2];
    }
    *(int *)(((int)c + 0xe0)) += 1;
    Vec3_Sub(sp8, c + 0xc8, (char *)((int *)WALKER(c)->mPath)[1] + WALKER(c)->mPathPoint * 0xc);
    WALKER(c)->mDir.x = sp8[0];
    WALKER(c)->mDir.y = sp8[1];
    WALKER(c)->mDir.z = sp8[2];
    if (NormalizeVec3IfNonZero(c+0xd4) == 0) {
        WALKER(c)->mDir.x = 0xb50;
        WALKER(c)->mDir.y = 0xb50;
        WALKER(c)->mDir.z = 0;
    }
    {
        int t = WALKER(c)->mPathPoint * 0xc;
        int *arr = (int *)((int *)WALKER(c)->mPath)[1];
        int* el = (int*)((char*)arr + t);
        sp14[0] = el[0];
        sp14[1] = el[1];
        sp14[2] = el[2];
    }
    WALKER(c)->mTargetAngle = (short)Vec3_HorzAngle(c + 0xc8, sp14);
    {
        int a = pmf_MascotWalker_StateWalk[0];
        int b = pmf_MascotWalker_StateWalk[1];
        WALKER(c)->mState[0] = b ? a : a;
        WALKER(c)->mState[1] = b;
    }
}
}
}

namespace s020c0364 {
extern "C" {
typedef short s16;
typedef long long s64;

typedef struct { int x, y, z; } Vector3;


void MascotWalker_StateWalk(char *c)
{
    char *path;
    char *arr;
    int idx;
    int a, b;
    int t1, t2;
    int r1, r2;

    _Z11UpdateAngleRssis((short *)(c + 0xea), WALKER(c)->mTargetAngle, 8, 0x100);

    a = WALKER(c)->mDir.x;
    if (a < 0)
        a = -a;
    t1 = FX_MUL(a, 0x1a9);

    b = WALKER(c)->mDir.z;
    if (b < 0)
        b = -b;

    idx = WALKER(c)->mPathPoint * 0xc;
    path = *(char **)(c + 0xb0);
    arr = *(char **)(path + 4);
    r1 = _Z14ApproachLinearRiii((int *)(c + 0xc8), *(int *)(arr + idx), t1);

    t2 = FX_MUL(b, 0x1a9);

    idx = WALKER(c)->mPathPoint * 0xc;
    path = *(char **)(c + 0xb0);
    arr = *(char **)(path + 4);
    r2 = _Z14ApproachLinearRiii((int *)(c + 0xd0), *(int *)(arr + idx + 8), t2);

    if (r1 != 0 && r2 != 0) {
        if (WALKER(c)->mPathPoint < *(int *)(*(char **)(c + 0xb0)) - 1) {
            Vector3 dir;
            Vector3 next;
            int *p;

            p = (int *)(c + 0xe0);
            *p = *p + 1;

            path = *(char **)(c + 0xb0);
            idx = WALKER(c)->mPathPoint * 0xc;
            arr = *(char **)(path + 4);
            Vec3_Sub(&dir, &WALKER(c)->mPos, (Vector3 *)(arr + idx));

            WALKER(c)->mDir.x = dir.x;
            WALKER(c)->mDir.y = dir.y;
            WALKER(c)->mDir.z = dir.z;

            if (NormalizeVec3IfNonZero(&WALKER(c)->mDir) == 0) {
                WALKER(c)->mDir.x = 0xb50;
                WALKER(c)->mDir.y = 0xb50;
                WALKER(c)->mDir.z = 0;
            }

            idx = WALKER(c)->mPathPoint * 0xc;
            path = *(char **)(c + 0xb0);
            arr = *(char **)(path + 4);
            next.x = *(int *)(arr + idx);
            next.y = *(int *)(arr + idx + 4);
            next.z = *(int *)(arr + idx + 8);

            WALKER(c)->mTargetAngle = Vec3_HorzAngle((const Vector3 *)(c + 0xc8), &next);
            return;
        }

        MascotWalker_StartNextPath(c);
        return;
    }

    if (_Z15ApproachLinear2Rsss((short *)(c + 0xf2), 0, 1) == 0)
        return;

    if (_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x68, 0) == 0)
        return;

    MascotWalker_StartPause((int *)c);
}
}
}

namespace s020c0304 {
extern "C" {
struct P2 { int w[2]; };
extern "C" { extern P2 pmf_MascotWalker_StatePause; }

void MascotWalker_StartPause(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x18, (void *)t[3], 8, 0, 0x800, 0);
    *(short *)((char *)t + 0xf2) = 0x78;
    t[0x3d] = 1;
    *(P2 *)((char *)t + 0xb4) = pmf_MascotWalker_StatePause;
}
}
}

namespace s020c0264 {
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim {
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */

struct P2 { int w[2]; };

extern "C" { extern int data_0209e650; }
extern "C" { extern P2 pmf_MascotWalker_StateWalk_3; }

extern "C" void MascotWalker_StatePause(char *c)
{
    if (ApproachLinear2(*(short *)(c + 0xf2), 0, 1) == 0)
        return;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x18), *(void **)(c + 0x14), 0, 0, 0x800, 0);
    int r = RandomIntInternal(&data_0209e650);
    int idx = (int)((unsigned int)(r & 0x7fffffff) >> 0x13);
    *(short *)(c + 0xf2) = (short)(((idx * 0x258) >> 12) + 0x258);
    *(int *)(c + 0xf4) = 0;
    *(P2 *)(c + 0xb4) = pmf_MascotWalker_StateWalk_3;
}
}

namespace s020c0134 {
extern "C" {
// @symbol Camera_UpdateMatrices
/* Camera_UpdateMatrices at 0x020c0134
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
typedef int s32;


struct Matrix4x3_local { int data[12]; };

struct dCamera_c {
    struct Matrix4x3_local viewMat;  /* 0x00 */
    char pad30[0x30];          /* 0x30 */
    struct Matrix4x3_local projMat;  /* 0x60 */
    char pad90[0x10];          /* 0x90 */
    struct Vector3 eye;        /* 0xa0 */
    struct Vector3 target;     /* 0xac */
    short angle;               /* 0xb8 */
};


extern short data_02082214[];
extern char data_0209f43c[];

void Camera_UpdateMatrices(struct dCamera_c *self)
{
    struct Vector3 up;
    struct Vector3 v1c;
    struct Vector3 v28;
    struct Vector3 v34;
    struct Vector3 v40;
    int idx;

    up.x = 0;
    up.y = 0x1000;
    up.z = 0;

    SubVec3(&self->eye, &self->target, &v1c);
    if (NormalizeVec3IfNonZero(&v1c)) {
        func_0203cebc(&v34, &v28, &v1c, &up);
        if (NormalizeVec3IfNonZero(&v28)) {
            func_0203cebc(&v40, &up, &v28, &v1c);
        } else {
            up.x = 0;
            up.y = 0;
            up.z = 0x1000;
        }
    }

    idx = (self->angle >> 4) * 2;
    _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
        data_02082214[idx], data_02082214[idx + 1],
        0x1555, 0x1000, 0x1388000, 0x1000, 1, &self->projMat);

    G3i::LookAt_(&self->target, &up, &self->eye, true, (Matrix4x3 *)&self->viewMat);

    _Z13CopyToViewMatPK9Matrix4x3(&self->viewMat);

    _ZN8dClipper13Func_020156DCEitii(
        data_0209f43c, 0x1555, self->angle, 0x1000, 0x1388000);
}
}
}

namespace s020bfff8 {
extern "C" {
extern short data_02082214[];

void Camera_ProjectToScreen(char* r4, void* r1, int* r6, int* r5) {
    int out[3];
    MulVec3Mat4x3(r1, r4, out);
    int ang = *(short*)(r4 + 0xb8) >> 4;
    int i = ang * 2;
    int cosv = data_02082214[i];
    int sinv = data_02082214[i + 1];
    int f1 = _ZN4cstd4fdivEii(cosv, sinv);
    int r = FX_MUL(f1, -out[2]);
    int f2 = _ZN4cstd4fdivEii(out[1], r);
    int t1 = FX_MUL(f2, 0x5f800u);
    r5[0] = -((t1 + (int)0xfffa0800) >> 0xc);
    int g = FX_MUL(r, 0x1555u);
    int f3 = _ZN4cstd4fdivEii(out[0], g);
    int t3 = FX_MUL(f3, 0x7f800u);
    r6[0] = (t3 + 0x7f800) >> 0xc;
}
}
}

namespace s020bfec0 {
extern "C" {
extern short data_02082214[];

void Camera_ProjectToScreen16(char* r4, void* r1, short* r5) {
    int out[3];
    MulVec3Mat4x3(r1, r4, out);
    int ang = *(short*)(r4 + 0xb8) >> 4;
    int i = ang * 2;
    int cosv = data_02082214[i];
    int sinv = data_02082214[i + 1];
    int f1 = _ZN4cstd4fdivEii(cosv, sinv);
    int r = FX_MUL(f1, -out[2]);
    int f2 = _ZN4cstd4fdivEii(out[1], r);
    int t1 = FX_MUL(f2, 0x5f800u);
    r5[1] = -((t1 + (int)0xfffa0800) >> 0xc);
    int g = FX_MUL(r, 0x1555u);
    int f3 = _ZN4cstd4fdivEii(out[0], g);
    int t3 = FX_MUL(f3, 0x7f800u);
    r5[0] = (t3 + 0x7f800) >> 0xc;
}
}
}
