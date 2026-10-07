//cpp
/*
 * ov006 .text 0x020bfec0..0x020c4048, 93 functions: the first linker unit of
 * the overlay, from the .text base up to the next unit at 0x020c4048. The ROM
 * carries no RTTI, vtable or static initialiser that names a type in this
 * run, and no source or note names it, so it keeps the unit<addr> name and
 * the functions keep their address names.
 *
 * Data the run touches (all of it is in this overlay, and none of it is
 * referenced from any other linker unit): .data/.rodata words in
 * 0x0213ac48..0x0213aee8, 0x0213ad28, 0x0212b890, 0x0212b89c, 0x0212b8fc and
 * 0x0212c9e4, and .bss 0x021402e0..0x021402fc.
 *
 * Folded from 93 one-function shards, func_ov006_020bfec0 through
 * func_ov006_020c402c plus Camera_UpdateMatrices (0x020c0134). The
 * boundaries inside the run could not be proven (no sinit, RTTI or
 * bss/destructor evidence separates anything in it), so it is folded as the
 * linker unit tools/tu_map.py reports.
 *
 * Layout of this file: one block of declarations for the external symbols,
 * then each function in its own namespace. The functions describe the same
 * actor memory with different recovered layouts, so each namespace holds the
 * layout structs and data externs that one function needed; the functions
 * themselves are extern "C" and the namespaces change no symbol. Functions
 * are written highest address first, since mwccarm emits .text in reverse
 * source order.
 */
#include "types.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "PlayerInput.h"

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
int _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(void *, void *, void *, int, void *);
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
void func_ov006_020bfec0(char* r4, void* r1, short* r5);
void func_ov006_020bfff8(char* r4, void* r1, int* r6, int* r5);
void Camera_UpdateMatrices(void *self);
void func_ov006_020c0264(char *c);
void func_ov006_020c0304(int *t);
void func_ov006_020c0364(char *c);
void func_ov006_020c057c(char* c);
void func_ov006_020c06dc(char* thiz);
void func_ov006_020c07a0(char *t);
void func_ov006_020c07e8(void * c);
void func_ov006_020c092c(char* thiz);
int func_ov006_020c09f8(char *t);
int func_ov006_020c0a48(char *t);
void func_ov006_020c0aa8(void *self);
void func_ov006_020c0af8(char* c);
void func_ov006_020c0b74(char *p);
void func_ov006_020c0c80(void *c);
void func_ov006_020c0ce8(char *c);
void func_ov006_020c0d68(char *c);
void func_ov006_020c0df0(char *c);
void func_ov006_020c0e8c(int *t);
int func_ov006_020c0efc(void* a);
int func_ov006_020c0f0c(int *c);
void func_ov006_020c0f9c(void *cc);
void func_ov006_020c1164(int *t, int a1, int a2);
void func_ov006_020c11c0(char *c);
void func_ov006_020c1420(char *c, short arg1, int arg2);
void func_ov006_020c14bc(char* c);
void func_ov006_020c1604(char* c, int unused, short a2, int a3);
int func_ov006_020c16b4(char *c);
int func_ov006_020c1718(int* r0);
void func_ov006_020c1760(void);
void func_ov006_020c1764(char *c);
void func_ov006_020c1804(void *self);
void func_ov006_020c19d0(char* thiz);
int func_ov006_020c1a88(char *c);
int func_ov006_020c1c64(char *t); // local extern: raw char*/void* view of the actor memory; the class header declares the typed form and would change call-site codegen
int func_ov006_020c1d80(char *t); // local extern: raw char*/void* view of the actor memory; the class header declares the typed form and would change call-site codegen
void func_ov006_020c1eb4(void *self);
void func_ov006_020c1ef8(int *p);
void func_ov006_020c1f04(char* c, int* src);
void func_ov006_020c1f4c(char* c);
void func_ov006_020c201c(void *c);
void func_ov006_020c2144(void* a);
void func_ov006_020c2154(char *c);
int func_ov006_020c21e4(char *t); // local extern: raw char*/void* view of the actor memory; the class header declares the typed form and would change call-site codegen
int func_ov006_020c221c(char *t);
void func_ov006_020c225c(void *self);
void func_ov006_020c2290(char* c);
void func_ov006_020c22d8(char *t);
void func_ov006_020c2300(void *c);
void func_ov006_020c23a8(void* c);
void func_ov006_020c2440(char* c);
void func_ov006_020c24e4(void* c);
void func_ov006_020c2594(void *c);
void func_ov006_020c263c(char *t);
void func_ov006_020c2664(char *c);
void func_ov006_020c26f4(char *t);
void func_ov006_020c271c(void *c);
void func_ov006_020c27c4(char* c);
void func_ov006_020c2848(char* c);
void func_ov006_020c2924(char* c);
int func_ov006_020c2984(void* a);
int func_ov006_020c2994(void * c);
void func_ov006_020c29dc(char *c);
void func_ov006_020c2b8c(void * c);
void func_ov006_020c2be8(char* c);
int func_ov006_020c3050(char *c);
int func_ov006_020c3288(char *t); // local extern: raw char*/void* view of the actor memory; the class header declares the typed form and would change call-site codegen
int func_ov006_020c33dc(char *t);
void func_ov006_020c3528(char* c);
void func_ov006_020c35a8(char* c);
void func_ov006_020c35e8(char* self);
void func_ov006_020c3754(int *r6, int *r5, int *r4);
void func_ov006_020c3884(char *t, int a1);
void func_ov006_020c38ac(void);
void func_ov006_020c38b0(char *p);
void func_ov006_020c3904(void);
void func_ov006_020c3908(char *p);
void func_ov006_020c395c(int* c);
void func_ov006_020c3990(char *c);
void func_ov006_020c3ad8(void);
void func_ov006_020c3adc(char *p);
void func_ov006_020c3b2c(void *self);
int func_ov006_020c3b80(int* c);
void func_ov006_020c3bc8(void *a);
void func_ov006_020c3bf4(void *self);
void func_ov006_020c3d18(char *c);
int func_ov006_020c3d88(char *c);
int func_ov006_020c3e54(char *t);
void* func_ov006_020c3e70(char* c); // local extern: raw char*/void* view of the actor memory; the class header declares the typed form and would change call-site codegen
void* func_ov006_020c3f54(char* c);
int func_ov006_020c402c(char *t);}

// ---- func_ov006_020c402c.c ----
namespace s020c402c {
extern "C" {
int func_ov006_020c402c(char *t)
{
    _ZN5ModelC1Ev(t + 0x44);
    return (int)t;
}
}
}

// ---- func_ov006_020c3f54.cpp ----
namespace s020c3f54 {
extern "C" {
}

extern "C" void* func_ov006_020c3f54(char* c)
{
    __cxa_vec_ctor(c + 8, 0x16, 0x98, (void*)func_ov006_020c402c, (void*)func_ov006_020c3e54);
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

// ---- func_ov006_020c3e70.cpp ----
namespace s020c3e70 {
extern "C" {
}
extern "C" void* func_ov006_020c3e70(char* c){
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
  __cxa_vec_cleanup(c + 8, 0x16, 0x98, (void *)func_ov006_020c3e54);
  return c;
}
}

// ---- func_ov006_020c3e54.c ----
namespace s020c3e54 {
extern "C" {
int func_ov006_020c3e54(char *t)
{
    _ZN5ModelD1Ev(t + 0x44);
    return (int)t;
}
}
}

// ---- func_ov006_020c3d88.cpp ----
namespace s020c3d88 {
struct SharedFilePtr; struct BMD_File;
struct ModelBase { int d; };
extern "C" int func_ov006_020c3d88(char *c)
{
    {
        void* bmd = _ZN5Model8LoadFileER13SharedFilePtr((c + 0xd7c));
        _ZN9ModelBase7SetFileEP8BMD_Fileii((ModelBase*)(c + 0xd18), bmd, 1, -1);
    }
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr((c + 0xd8c));
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr((c + 0xd94));
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr((c + 0xd9c));
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr((c + 0xda4));
    func_ov006_020c3adc(c);
    int m = (int)_ZN5Model8LoadFileER13SharedFilePtr((c + 0xd84));
    int i = 0;
    int z = i;
    char *p = c + 8;
    for (; i < 0x16; i++) {
        func_ov006_020c3884(p, m);
        *(int *)(c + 0x48) = z;
        p += 0x98;
        c += 0x98;
    }
    return 1;
}
}

// ---- func_ov006_020c3d18.cpp ----
namespace s020c3d18 {
extern "C" {
void func_ov006_020c3d18(char *c)
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
            func_ov006_020c35e8(e);
        i++;
        c += 0x98;
        e += 0x98;
    } while (i < 0x16);
}
}
}

// ---- func_ov006_020c3bf4.cpp ----
namespace s020c3bf4 {
// @symbol func_ov006_020c3bf4
/* recovered: shared common types */

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

extern "C" void func_ov006_020c3bf4(void *self)
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
                func_ov006_020c35a8(p);
            c += 0x98;
            p += 0x98;
        }
    }
}
}

// ---- func_ov006_020c3bc8.c ----
namespace s020c3bc8 {
extern "C" {
void func_ov006_020c3bc8(void *a)
{
  int i = 0;
  char *p = (char *) a;
  for (; i < 22; i++)
  {
    *((int *) (p + 0x48)) = 0;
    p += 0x98;
  }
  func_ov006_020c3adc((char *)a);
}
}
}

// ---- func_ov006_020c3b80.c ----
namespace s020c3b80 {
extern "C" {
extern int data_ov006_0213aee0[];
int func_ov006_020c3b80(int* c){
  int* g=data_ov006_0213aee0;
  int r2=c[0];
  int ip=1;
  if(r2==g[0]){
    if(c[1]==g[1] || r2==0) ip=0;
  }
  return ip==0;
}
}
}

// ---- func_ov006_020c3b2c.c ----
namespace s020c3b2c {
extern "C" {
// @symbol func_ov006_020c3b2c
/* recovered: shared common types */
/* func_ov006_020c3b2c at 0x020c3b2c
 *
 * dCamera_c preset init: sets eye/target vectors and angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of func_ov006_020c225c.
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


void func_ov006_020c3b2c(struct dCamera_c *self)
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

// ---- func_ov006_020c3adc.c ----
namespace s020c3adc {
extern "C" {
struct S { int w[2]; };
extern "C" { extern struct S data_ov006_0213aec0; }
void func_ov006_020c3adc(char *p) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0xd18, *(void**)(p+0xd90), 0, 0x800, 0);
    *(struct S *)(p + 0x0) = data_ov006_0213aec0;
}
}
}

// ---- func_ov006_020c3ad8.c ----
namespace s020c3ad8 {
extern "C" {
void func_ov006_020c3ad8(void)
{
}
}
}

// ---- func_ov006_020c3990.c ----
namespace s020c3990 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
struct S8 { int w[2]; };


extern "C" { extern int data_0209e650; }
extern "C" { extern int data_ov006_0213aec8[2]; }

void func_ov006_020c3990(char *c)
{
    int i;
    char *p;
    struct { Vec3 pos, vel, pos2, vel2; } b;

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd18, *(void **)(c + 0xd98), 0x40000000, 0x800, 0);

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
            func_ov006_020c3754((int *)(c + 8 + i * 0x98), (int *)&b.pos2, (int *)&b.vel2);
            break;
        }
    }

    *(S8 *)c = *(S8 *)&data_ov006_0213aec8[0];
}
}
}

// ---- func_ov006_020c395c.c ----
namespace s020c395c {
extern "C" {
void func_ov006_020c395c(int* c){ if(_ZN15dExtFrameCtrl_c8FinishedEv((char*)c+0xd68)==0) return; func_ov006_020c3adc((char *)c); }
}
}

// ---- func_ov006_020c3908.c ----
namespace s020c3908 {
extern "C" {
struct S { int w[2]; };
extern "C" { extern struct S data_ov006_0213aed8; }
void func_ov006_020c3908(char *p) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0xd18, *(void**)(p+0xda0), 0x40000000, 0x800, 0);
    *(struct S *)(p + 0x0) = data_ov006_0213aed8;
}
}
}

// ---- func_ov006_020c3904.c ----
namespace s020c3904 {
extern "C" {
void func_ov006_020c3904(void)
{
}
}
}

// ---- func_ov006_020c38b0.c ----
namespace s020c38b0 {
extern "C" {
struct S { int w[2]; };
extern "C" { extern struct S data_ov006_0213aed0; }
void func_ov006_020c38b0(char *p) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0xd18, *(void**)(p+0xda8), 0x40000000, 0x800, 0);
    *(struct S *)(p + 0x0) = data_ov006_0213aed0;
}
}
}

// ---- func_ov006_020c38ac.c ----
namespace s020c38ac {
extern "C" {
void func_ov006_020c38ac(void)
{
}
}
}

// ---- func_ov006_020c3884.c ----
namespace s020c3884 {
extern "C" {
void func_ov006_020c3884(char *t, int a1)
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(t + 0x44, (void *)a1, 1, -1);
    *(int *)(t + 0x40) = 0;
}
}
}

// ---- func_ov006_020c3754.c ----
namespace s020c3754 {
extern "C" {
extern "C" { extern int data_ov006_021402e0; }
extern "C" { extern int data_ov006_021402e4; }
extern "C" { extern int data_ov006_021402f0[3]; }
extern "C" { extern int data_0209e650; }
extern "C" { extern int data_02092768[4]; }

void func_ov006_020c3754(int *r6, int *r5, int *r4)
{
    int *n;
    if (!(data_ov006_021402e0 & 1)) {
        n = data_ov006_021402f0;
        n[0] = 0;
        n[1] = 0x1000;
        n[2] = 0;
        func_020731dc(n, (void *)_ZN7Vector3D1Ev, (void **)&data_ov006_021402e4);
        data_ov006_021402e0 |= 1;
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
    r6[9] = data_ov006_021402f0[0];
    r6[10] = data_ov006_021402f0[1];
    r6[11] = data_ov006_021402f0[2];
    func_0203cd80((char *)r6 + 0x24, (short)RandomIntInternal(&data_0209e650));
    r6[12] = data_02092768[0];
    r6[13] = data_02092768[1];
    r6[14] = data_02092768[2];
    r6[15] = data_02092768[3];
    Quaternion_FromVector3((char *)r6 + 0x30, data_ov006_021402f0, (int *)((char *)r6 + 0x24));
    *(int *)((char *)r6 + 0x40) = 1;
}
}
}

// ---- func_ov006_020c35e8.c ----
namespace s020c35e8 {
extern "C" {
typedef struct { int x, y, z; } Vec3;
struct Quat { int w[4]; };


extern "C" { extern Quat data_02092768; }
extern int data_02082214[];

void func_ov006_020c35e8(char* self)
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
        *(int*)(self + 0x40) = 0;
        return;
    }
    AddVec3(self, self + 0xc, self);
    *(int*)(self + 0x1c) = -0x40;
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

// ---- func_ov006_020c35a8.cpp ----
namespace s020c35a8 {
// @symbol func_ov006_020c35a8
/* recovered: shared common types */
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
extern "C" void func_ov006_020c35a8(char* c) {
    func_ov006_020c3528(c);
    Vector3 v; v.z=0x1000; v.y=0x1000; v.x=0x1000;
    ((Obj*)(c + 0x44))->m5(&v);
}
}

// ---- func_ov006_020c3528.cpp ----
namespace s020c3528 {
// @symbol func_ov006_020c3528
/* recovered: shared common types */
extern "C" {
struct Q{int a,b,c,d;};


extern "C" { extern struct Vector3 data_ov006_0212c9e4; }
extern "C" { extern struct Matrix4x3 data_020a0e68; }
void func_ov006_020c3528(char* c){
  struct Matrix4x3 sp;
  Matrix4x3_FromQuaternion((char*)c+0x30, &sp);
  MulVec3Mat3x3(&data_ov006_0212c9e4, &sp, (char*)c+0x24);
  Matrix4x3_FromTranslation(&data_020a0e68, *(int*)c, *(int*)(c+4), *(int*)(c+8));
  MulMat4x3Mat4x3(&sp, &data_020a0e68, &data_020a0e68);
  *(struct Matrix4x3*)((char*)c+0x60) = data_020a0e68;
}
}
}

// ---- func_ov006_020c33dc.c ----
namespace s020c33dc {
extern "C" {
int func_ov006_020c33dc(char *t)
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
    *(int *)(t + 0x1a4) = 0;
    *(int *)(t + 0x1a8) = 0;
    return (int)t;
}
}
}

// ---- func_ov006_020c3288.c ----
namespace s020c3288 {
extern "C" {
int func_ov006_020c3288(char *t)
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

// ---- func_ov006_020c3050.c ----
namespace s020c3050 {
extern "C" {
extern "C" { extern char data_ov006_0213ae00[]; extern char data_ov006_0213ae18[]; extern char data_ov006_0213ae30[]; extern char data_ov006_0213ae48[]; extern char data_ov006_0213ae60[]; extern char data_ov006_0213ae78[]; extern char data_ov006_0213ae90[]; extern char data_ov006_0213aea8[]; }
// @symbol func_ov006_020c3050
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" { extern struct Matrix4x3 data_020a0e68; }



int func_ov006_020c3050(char *c)
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

    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x144));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x14c));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x154));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x15c));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x164));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x16c));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x174));
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(r5, *(void **)(c + 0x17c));

    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae48);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae60);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213aea8);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae00);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae30);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae78);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae90);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(r5, data_ov006_0213ae18);

    func_ov006_020c2848(c);
    *(int *)(c + 0x180) = 0;
    *(int *)(c + 0x184) = 0;
    *(int *)(c + 0x188) = 0;
    func_ov006_020c2290(c);

    if (!_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x78, r4, 1, -1))
        return 0;

    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
    *(struct Matrix4x3 *)(c + 0x94) = data_020a0e68;
    return 1;
}
}
}

// ---- func_ov006_020c2be8.c ----
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
            a = (int)(((s64)d * -0x100 + 0x800) >> 12); \
            b = (int)(((s64)d * -0x260 + 0x800) >> 12); \
        } else { \
            int d = (0x800 - q) << 1; \
            a = (int)(((s64)d * 0x260 + 0x800) >> 12); \
            b = (int)(((s64)d * 0x100 + 0x800) >> 12); \
        } \
        { \
            int m = (int)(((s64)r * 0x20 + 0x800) >> 12) + (int)(((s64)w * 0x500 + 0x800) >> 12); \
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

void func_ov006_020c2be8(char* c)
{
    if (*(int*)(c + 0x1a4) != 0) {
        int s = *(int*)(c + 0x19c);
        int t = *(int*)(c + 0x1a0);
        COMPUTE(c, s, t);
        return;
    }
    if (func_ov006_020c2994(c) != 0) {
        int i = gActivePlayerSlot;
        if (gTouchHeld[i * 4] != 0) {
            int s = gTouchX[i * 4];
            int t = gTouchY[i * 4];
            COMPUTE(c, s, t);
            return;
        }
    }
    *(int*)(c + 0x18c) = (int)(((s64)*(int*)(c + 0x18c) * 0xE00 + 0x800) >> 12);
    *(int*)(c + 0x190) = (int)(((s64)*(int*)(c + 0x190) * 0xE00 + 0x800) >> 12);
    *(int*)(c + 0x194) = (int)(((s64)*(int*)(c + 0x194) * 0xE00 + 0x800) >> 12);
    *(int*)(c + 0x198) = (int)(((s64)*(int*)(c + 0x198) * 0xE00 + 0x800) >> 12);
}
}
}

// ---- func_ov006_020c2b8c.cpp ----
namespace s020c2b8c {
extern "C" {
struct C;
typedef void (C::*PMF)();
struct C { PMF pmf; };
void func_ov006_020c2b8c(C* c) {
    func_ov006_020c2be8((char *)c);
    if (c->pmf) (c->*(c->pmf))();
    _ZN15dExtFrameCtrl_c7AdvanceEv((char*)c + 0xc8);
    _ZN15dExtFrameCtrl_c7AdvanceEv((char*)c + 0xdc);
    _ZN14BlendModelAnim7AdvanceEv((char*)c + 8);
    func_ov006_020c2290((char *)c);
}
}
}

// ---- func_ov006_020c29dc.cpp ----
namespace s020c29dc {
extern "C" {
extern "C" { extern char data_ov006_0212b8fc; }
}
struct Obj {
    void* vt;
    virtual void v0(); virtual void v1(); virtual void v2();
    virtual void v3(); virtual void v4();
    virtual void m(void* arg);
};
extern "C" void func_ov006_020c29dc(char *c)
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
    *(int *)(((int)p + 0x104)) -= *(int *)(c + 0x194);
    *(int *)(((int)p + 0x108)) += *(int *)(c + 0x198);
    *(int *)(((int)p + 0x134)) += *(int *)(c + 0x18c);
    *(int *)(((int)p + 0x138)) += *(int *)(c + 0x190);
    ((Obj *)(c + 8))->m(0);
    ((Obj *)(c + 0x78))->m(&data_ov006_0212b8fc);
}
}

// ---- func_ov006_020c2994.c ----
namespace s020c2994 {
extern "C" {
struct P{int x,y;};
extern "C" { extern struct P data_ov006_0213adb8; }
int func_ov006_020c2994(struct P* c){
  int ip=1;
  struct P* g=&data_ov006_0213adb8;
  if(c->x==g->x){
    if(c->y!=g->y && c->x!=0) ;
    else ip=0;
  }
  return ip==0;
}
}
}

// ---- func_ov006_020c2984.c ----
namespace s020c2984 {
extern "C" {
/* func_ov006_020c2984 @ 0x20c2984 (ov006) -- veneer: add r0,r0,#0x58; b dExtFrameCtrl_c::Finished(). */

int func_ov006_020c2984(void* a) {
    return _ZN15dExtFrameCtrl_c8FinishedEv((char*)a + 0x58);
}
}
}

// ---- func_ov006_020c2924.cpp ----
namespace s020c2924 {
extern "C" {
extern "C" { extern void* data_0209f5bc; }
void func_ov006_020c2924(char* c){
  func_ov006_020c2848(c);
  void* obj=data_0209f5bc;
  int (**vt)(void*)=*(int(***)(void*))obj;
  if(vt[6](obj)==0) return;
  *(int*)(c+0x190)=0;
  *(int*)(c+0x18c)=*(int*)(c+0x190);
  *(int*)(c+0x198)=0;
  *(int*)(c+0x194)=*(int*)(c+0x198);
  *(int*)(c+0x60)=0;
  *(int*)(c+0xd0)=0;
  *(int*)(c+0xe4)=0;
}
}
}

// ---- func_ov006_020c2848.cpp ----
namespace s020c2848 {
extern "C" {
extern "C" { extern void* data_ov006_0213ae48; }
extern "C" { extern void* data_0209f5bc; }
struct P2 { void* a; void* b; };
extern "C" { extern P2 data_ov006_0213adc8; }
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

extern "C" void func_ov006_020c2848(char* c)
{
    if (*(int*)(c + 0x68) != *(int*)(c + 0x104)) {
        int r5 = 0;
        if (((VObj*)data_0209f5bc)->v6() == 0) {
            if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x58) == 0 || *(int*)(c + 0x68) == *(int*)(c + 0x13c)) {
                r5 = 8;
            }
        }
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 8, *(void**)(c + 0x104), r5, 0, 0x800, 0);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0xc8, *(void**)(c + 0x144), 0, 0x800, 0);
        _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0xdc, (void*)&data_ov006_0213ae48, 0, 0x800, 0);
    }
    {
        void* lo = ((volatile P2*)&data_ov006_0213adc8)->a;
        void* hi = ((volatile P2*)&data_ov006_0213adc8)->b;
        *(void**)(c) = lo;
        *(void**)(c + 4) = hi;
    }
}
}

// ---- func_ov006_020c27c4.cpp ----
namespace s020c27c4 {
extern "C" {
extern "C" { extern void* data_ov006_0213ae48; }
void func_ov006_020c27c4(char* c){
  if (!_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x58)) return;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 8, *(void**)(c + 0x104), 0, 0, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0xc8, *(void**)(c + 0x144), 0, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0xdc, (void*)&data_ov006_0213ae48, 0, 0x800, 0);
}
}
}

// ---- func_ov006_020c271c.c ----
namespace s020c271c {
extern "C" {
extern "C" { extern int data_ov006_0213aea8; }
extern "C" { extern long long data_ov006_0213ade0; }
inline char *inline_fn(char *arg0)
{
  return arg0 + 8;
}

void func_ov006_020c271c(void *c)
{
  char *r4 = (char *) c;
  unsigned long long new_var;
  long long *new_var2;
  if ((*((int *) (r4 + 0x1a8))) == 0)
  {
    func_02012174(0, 0x1c);
  }
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(inline_fn(r4), *((void **) (r4 + 0x114)), 4, 0x40000000, 0x800, 0);
  new_var2 = (long long *) r4;
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(r4 + 0xc8, *((void **) (r4 + 0x154)), 0x40000000, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &data_ov006_0213aea8, 0x40000000, 0x800, 0);
  new_var = data_ov006_0213ade0;
  *new_var2 = new_var;
}
}
}

// ---- func_ov006_020c26f4.c ----
namespace s020c26f4 {
extern "C" {
void func_ov006_020c26f4(char *t)
{
    int r = func_ov006_020c2984(t);
    if (r == 0) return;
    func_ov006_020c2848(t);
}
}
}

// ---- func_ov006_020c2664.c ----
namespace s020c2664 {
extern "C" {
extern int data_ov006_0213ae60[];
extern int data_ov006_0213adf0[];

struct W2 { int w[2]; };

void func_ov006_020c2664(char *c)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 8, *(void **)(c + 0x10c), 4, 0x40000000, 0x800, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0xc8, *(void **)(c + 0x14c), 0x40000000, 0x800, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0xdc, data_ov006_0213ae60, 0x40000000, 0x800, 0);
    *(struct W2 *)c = *(struct W2 *)data_ov006_0213adf0;
}
}
}

// ---- func_ov006_020c263c.c ----
namespace s020c263c {
extern "C" {
void func_ov006_020c263c(char *t)
{
    int r = func_ov006_020c2984(t);
    if (r == 0) return;
    func_ov006_020c2848(t);
}
}
}

// ---- func_ov006_020c2594.c ----
namespace s020c2594 {
extern "C" {
extern "C" { extern int data_ov006_0213ae90; }
extern "C" { extern volatile long long data_ov006_0213adc0; }
void func_ov006_020c2594(void *c)
{
  char *r4 = (char *) c;
  if ((*((int *) (r4 + 0x1a8))) == 0)
  {
    func_02012174(0, 3);
  }
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(r4 + 8, *((void **) (r4 + 0x13c)), 8, 0x40000000, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(r4 + 0xc8, *((void **) (r4 + 0x17c)), 0x40000000, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &data_ov006_0213ae90, 0x40000000, 0x800, 0);
  *((long long *) r4) = data_ov006_0213adc0;
}
}
}

// ---- func_ov006_020c24e4.cpp ----
namespace s020c24e4 {
extern "C" {
extern "C" { extern int data_ov006_0213ae18; }
void func_ov006_020c24e4(void* c){
  char* r4 = (char*)c;
  if (!func_ov006_020c2984(r4)) return;
  if (*(int*)(r4 + 0x68) != *(int*)(r4 + 0x13c)) return;
  if (*(int*)(r4 + 0x1a8) == 0) _ZN5Sound12PlayBank2_2DEj(0x1d1);
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(r4 + 8, *(void**)(r4 + 0x134), 8, 0, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(r4 + 0xc8, *(void**)(r4 + 0x174), 0, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &data_ov006_0213ae18, 0, 0x800, 0);
}
}
}

// ---- func_ov006_020c2440.cpp ----
namespace s020c2440 {
extern "C" {
struct G2 { int w[2]; };
extern "C" { extern struct G2 data_ov006_0213adf8; }
extern "C" { extern void* data_ov006_0213ae30; }
void func_ov006_020c2440(char* c){
  if (*(int*)(c + 0x1a8) == 0) _ZN5Sound12PlayBank2_2DEj(0x1c9);
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 8, *(void**)(c + 0x124), 0, 0x40000000, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0xc8, *(void**)(c + 0x164), 0x40000000, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0xdc, (void*)&data_ov006_0213ae30, 0x40000000, 0x800, 0);
  *(struct G2*)c = data_ov006_0213adf8;
}
}
}

// ---- func_ov006_020c23a8.cpp ----
namespace s020c23a8 {
extern "C" {
extern "C" { extern int data_ov006_0213ae00; }
void func_ov006_020c23a8(void* c){
  char* r4 = (char*)c;
  if (!func_ov006_020c2984(r4)) return;
  if (*(int*)(r4 + 0x68) != *(int*)(r4 + 0x124)) return;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(r4 + 8, *(void**)(r4 + 0x11c), 0, 0, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(r4 + 0xc8, *(void**)(r4 + 0x15c), 0, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &data_ov006_0213ae00, 0, 0x800, 0);
}
}
}

// ---- func_ov006_020c2300.c ----
namespace s020c2300 {
extern "C" {
extern "C" { extern int data_ov006_0213ae90; }
extern "C" { extern long long data_ov006_0213add0; }
void func_ov006_020c2300(void *c)
{
  long long new_var;
  char *r4 = (char *) c;
  if ((*((int *) (((char *) c) + 0x1a8))) == 0)
  {
    func_02012174(0, 3);
  }
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(r4 + 8, *((void **) (r4 + 0x13c)), 8, 0x40000000, 0x800, 0);
  _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(r4 + 0xc8, *((void **) (r4 + 0x17c)), 0x40000000, 0x800, 0);
  _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(r4 + 0xdc, &data_ov006_0213ae90, 0x40000000, 0x800, 0);
  *((long long *) r4) = (new_var = data_ov006_0213add0 & 0xFFFFFFFFFFFFFFFF);
}
}
}

// ---- func_ov006_020c22d8.c ----
namespace s020c22d8 {
extern "C" {
void func_ov006_020c22d8(char *t)
{
    int r = func_ov006_020c2984(t);
    if (r == 0) return;
    func_ov006_020c2848(t);
}
}
}

// ---- func_ov006_020c2290.c ----
namespace s020c2290 {
extern "C" {
// @symbol func_ov006_020c2290
/* recovered: shared common types */
extern "C" { extern struct Matrix4x3 data_020a0e68; }
void func_ov006_020c2290(char* c) {
    Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c+0x180), *(int*)(c+0x184), *(int*)(c+0x188));
    *(struct Matrix4x3*)(c+0x24) = data_020a0e68;
}
}
}

// ---- func_ov006_020c225c.c ----
namespace s020c225c {
extern "C" {
// @symbol func_ov006_020c225c
/* recovered: shared common types */
/* func_ov006_020c225c at 0x020c225c
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


void func_ov006_020c225c(struct dCamera_c *self)
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

// ---- func_ov006_020c221c.c ----
namespace s020c221c {
extern "C" {
int func_ov006_020c221c(char *t)
{
    func_02017acc(t, 0x205);
    _ZN13SharedFilePtr9ConstructEj(t + 8, 0x206);
    _ZN14BlendModelAnimC1Ev(t + 0x10);
    *(int *)(t + 0xa8) = 0;
    return (int)t;
}
}
}

// ---- func_ov006_020c21e4.c ----
namespace s020c21e4 {
extern "C" {
int func_ov006_020c21e4(char *t)
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

// ---- func_ov006_020c2154.c ----
namespace s020c2154 {
extern "C" {
void func_ov006_020c2154(char *c)
{
    void *f;
    f = _ZN5Model8LoadFileER13SharedFilePtr(c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x10, f, 1, -1);
    _ZN15dExtFrameCtrl_c8LoadFileER13SharedFilePtr(c + 8);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x10, *(void **)(c + 0xc), 0, 0, 0x800, 0);
    _ZN5Model12HideMaterialEii(c + 0x10, 7, 0);
    *(int *)(c + 0x8c) = 0xd000;
    *(int *)(c + 0x90) = 0;
    *(int *)(c + 0x94) = 0;
    *(short *)(c + 0x98) = 0;
    *(short *)(c + 0x9a) = -0x1400;
    *(short *)(c + 0x9c) = 0;
}
}
}

// ---- func_ov006_020c2144.c ----
namespace s020c2144 {
extern "C" {
/* func_ov006_020c2144 @ 0x20c2144 (ov006) -- veneer: add r0,r0,#0x10; b BlendModelAnim::Advance(). */

void func_ov006_020c2144(void* a) {
    _ZN14BlendModelAnim7AdvanceEv((char*)a + 0x10);
}
}
}

// ---- func_ov006_020c201c.cpp ----
namespace s020c201c {
typedef short s16;


struct C {
    char pad10[0x10];
    Model model;
    char pad[0x80 - 0x14];
    int v80;
    char pad8c[0x8c - 0x84];
    int v8c;
    char pad9a[0x9a - 0x90];
    s16 v9a;
    char pada2[0xa2 - 0x9c];
    s16 va2;
    s16 va4;
    char pada8[0xa8 - 0xa6];
    int va8;
};


extern "C" void func_ov006_020c201c(C *c)
{
    if (c->va8 != 0) {
        s16 h = Vec3_HorzAngle((const Vector3 *)&c->v8c, (const Vector3 *)&c->v80);
        s16 v = Vec3_VertAngle((const Vector3 *)&c->v8c, (const Vector3 *)&c->v80);
        s16 dh = (s16)(h - c->v9a);
        if (dh < -0x2800) dh = -0x2800;
        else if (dh > 0x2800) dh = 0x2800;
        if (v < -0x1000) v = -0x1000;
        else if (v > 0x1000) v = 0x1000;
        UpdateAngle(c->va4, dh, 8, 0x200);
        UpdateAngle(c->va2, v, 8, 0x200);
        dh = (s16)(h - dh);
        UpdateAngle(c->v9a, dh, 8, 0x200);
    } else {
        UpdateAngle(c->va2, 0, 8, 0x200);
        UpdateAngle(c->va4, 0x800, 8, 0x200);
        UpdateAngle(c->v9a, -0x1400, 8, 0x200);
    }
    func_ov006_020c1f4c((char *)c);
    c->model.Render(0);
}
}

// ---- func_ov006_020c1f4c.cpp ----
namespace s020c1f4c {
// @symbol func_ov006_020c1f4c
/* recovered: shared common types */
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

extern "C" void func_ov006_020c1f4c(char* c){
  Matrix4x3_FromTranslation(&data_020a0e68, *(int*)(c+0x8c), *(int*)(c+0x90), *(int*)(c+0x94));
  Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16*)(c+0x9a));
  *(Matrix4x3*)(c+0x2c) = data_020a0e68;
  ((Vtbl*)(c+0x10))->v3();
  data_020a0e68 = (*(Sub**)(c+0x24))->m;
  Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16*)(c+0xa4));
  Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(s16*)(c+0xa2));
  (*(Sub**)(c+0x24))->m = data_020a0e68;
}
}

// ---- func_ov006_020c1f04.c ----
namespace s020c1f04 {
extern "C" {
void func_ov006_020c1f04(char* c, int* src) {
    if (src != 0) {
        *(int*)(c+0x80) = src[0];
        *(int*)(c+0x84) = src[1];
        *(int*)(c+0x88) = src[2];
    } else {
        *(int*)(c+0x80) = 0;
        *(int*)(c+0x84) = 0x2000;
        *(int*)(c+0x88) = -0x2000;
    }
    *(int*)(c+0xa8) = 1;
}
}
}

// ---- func_ov006_020c1ef8.c ----
namespace s020c1ef8 {
extern "C" {
void func_ov006_020c1ef8(int *p)
{
    p[42] = 0;
}
}
}

// ---- func_ov006_020c1eb4.c ----
namespace s020c1eb4 {
extern "C" {
// @symbol func_ov006_020c1eb4
/* recovered: shared common types */
/* func_ov006_020c1eb4 at 0x020c1eb4
 *
 * dCamera_c preset init: sets eye/target vectors and angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of func_ov006_020c225c.
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


void func_ov006_020c1eb4(struct dCamera_c *self)
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

// ---- func_ov006_020c1d80.c ----
namespace s020c1d80 {
extern "C" {
int func_ov006_020c1d80(char *t)
{
    *(short *)(t + 0x1a) = 0;
    _ZN14BlendModelAnimC1Ev(t + 0x1c);
    _ZN5ModelC1Ev(t + 0x8c);
    func_ov006_020c0a48(t + 0xdc);
    *(short *)(t + 0x1dc) = 0;
    *(short *)(t + 0x1e6) = 0;
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
    *(int *)(t + 0x260) = 0;
    *(int *)(t + 0x264) = 0;
    *(int *)(t + 0x268) = 0;
    *(int *)(t + 0x26c) = 0;
    return (int)t;
}
}
}

// ---- func_ov006_020c1c64.c ----
namespace s020c1c64 {
extern "C" {
int func_ov006_020c1c64(char *t)
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
    func_ov006_020c09f8(t + 0xdc);
    _ZN5ModelD1Ev(t + 0x8c);
    _ZN14BlendModelAnimD1Ev(t + 0x1c);
    return (int)t;
}
}
}

// ---- func_ov006_020c1a88.c ----
namespace s020c1a88 {
extern "C" {
// @symbol func_ov006_020c1a88
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" { extern struct Vector3 data_020a0ebc; }
extern "C" { extern struct Vector3_16 data_020a0edc; }


int func_ov006_020c1a88(char *c)
{
    void *f;
    void *ip;

    _ZN17dExtShadowModel_c8CleanAllEv();
    *(void **)(c + 0x260) = _ZN5Model8LoadFileER13SharedFilePtr(c + 0x1e8);
    *(void **)(c + 0x264) = _ZN5Model8LoadFileER13SharedFilePtr(c + 0x1f0);
    f = *(void **)(c + 0x260);
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
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void **)(c + 0x1fc), 0, 0, 0x800, 0);
    func_ov006_020c0af8(c);
    *(int *)(c + 8) = data_020a0ebc.x;
    *(int *)(c + 0xc) = data_020a0ebc.y;
    *(int *)(c + 0x10) = data_020a0ebc.z;
    *(short *)(c + 0x14) = data_020a0edc.x;
    *(short *)(c + 0x16) = data_020a0edc.y;
    *(short *)(c + 0x18) = data_020a0edc.z;
    *(int *)(c + 0x1d4) = 0;
    _ZN5Model12HideMaterialEii(c + 0x1c, 1, 0);
    func_ov006_020c1764(c);
    f = *(void **)(c + 0x264);
    if (f == 0 || _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x8c, f, 1, -1) == 0) {
        return 0;
    }
    ip = *(void **)(c + 0x98);
    *(int *)((char *)ip + 0x24) &= ~1;
    *(int *)((char *)ip + 0x114) &= ~1;
    *(int *)((char *)ip + 0xe4) |= 2;
    func_ov006_020c092c(c + 0xdc);
    *(short *)(c + 0x1e0) = 0;
    *(short *)(c + 0x1e2) = 1;
    *(short *)(c + 0x1e4) = 0;
    return 1;
}
}
}

// ---- func_ov006_020c19d0.cpp ----
namespace s020c19d0 {
struct C;
typedef void (C::*PMF)();
struct C {
    PMF pmf;
};
extern "C" { extern short data_ov006_0212b89c[]; }

extern "C" void func_ov006_020c19d0(char* thiz)
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
    func_ov006_020c07e8(c + 0xdc);
}
}

// ---- func_ov006_020c1804.cpp ----
namespace s020c1804 {
// @symbol func_ov006_020c1804
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */

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

extern "C" void func_ov006_020c1804(void *self)
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

    ang = *(short*)(c + 0x1e4);
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

    ang = *(short*)(c + 0x1e2);
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
    func_ov006_020c07a0(c + 0xdc);
    ((Obj*)(c + 0x8c))->f5(0);

    dExtShadowModel_c::RenderAll();
}
}

// ---- func_ov006_020c1764.cpp ----
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

extern "C" { extern P2 data_ov006_0213ac50; }

extern "C" void func_ov006_020c1764(char *c)
{
    if (*(short *)(c + 0x1a) == 1) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), *(void **)(c + 0x234), 0, 0, 0x800, 0);
        if (*(int *)(c + 0x26c) == 0) {
            Sound::PlayBank2_2D(0x13a);
            *(int *)(c + 0x26c) = 1;
        }
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), *(void **)(c + 0x1fc), 0, 0, 0x800, 0);
    }
    *(P2 *)c = data_ov006_0213ac50;
}
}

// ---- func_ov006_020c1760.c ----
namespace s020c1760 {
extern "C" {
void func_ov006_020c1760(void)
{
}
}
}

// ---- func_ov006_020c1718.c ----
namespace s020c1718 {
extern "C" {
extern int data_ov006_0213ac58[];
int func_ov006_020c1718(int* r0) {
    int* g = data_ov006_0213ac58;
    int w0 = r0[0];
    int ip = 1;
    if (w0 == g[0]) {
        if (r0[1] == g[1] || w0 == 0) ip = 0;
    }
    return ip == 0;
}
}
}

// ---- func_ov006_020c16b4.c ----
namespace s020c16b4 {
extern "C" {
extern int data_ov006_0213ac90[];
int func_ov006_020c16b4(char *c)
{
    int *G = data_ov006_0213ac90;
    int r3 = *(int *)c;
    int r4 = 0;
    int lr = 1;
    if (r3 == G[0] && (*(int *)(c + 4) == G[1] || r3 == 0))
        lr = 0;
    if (lr == 0 && *(int *)(c + 0x7c) == *(int *)(c + 0x1fc))
        r4 = 1;
    return r4;
}
}
}

// ---- func_ov006_020c1604.c ----
namespace s020c1604 {
extern "C" {
typedef int Fix12;
typedef short s16;
struct M8 { int w[2]; };
extern char data_ov006_0213ac60[];

void func_ov006_020c1604(char* c, int unused, short a2, int a3) {
  if (*(s16*)(c+0x1dc) == 0) {
    *(s16*)(c+0x1d8) = 4;
    *(s16*)(c+0x1da) = 3;
  } else {
    *(s16*)(c+0x1d8) = 1;
    *(s16*)(c+0x1da) = 1;
  }
  if (_Z15ApproachLinear2Rsss((short*)(c+0x1dc), 5, 1) != 0) {
    *(s16*)(c+0x1dc) = 0;
  }
  *(int*)(c+0x26c) = 0;
  *(s16*)(c+0x1de) = a2;
  *(int*)(c+0x1d4) = a3;
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x1c, *(void**)(c+0x204), 0, 0x40000000, 0x800, 0);
  *(struct M8*)c = *(struct M8*)data_ov006_0213ac60;
}
}
}

// ---- func_ov006_020c14bc.cpp ----
namespace s020c14bc {
struct V {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21();
};


extern "C" { extern V* data_ov004_020beb68; }

extern "C" void func_ov006_020c14bc(char* c)
{
    if (*(int*)(c + 0x268) == 0) {
        _ZN5Sound12PlayBank2_2DEj(0x139);
        *(int*)(c + 0x268) = 1;
    }

    if (*(int*)(c + 0x7c) == *(int*)(c + 0x204)) {
        if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x6c))
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void**)(c + 0x20c), 0, 0, 0x800, 0);
        return;
    }

    if (_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 0)) {
        short cv = *(short*)(c + 0x100 + 0xd8);
        short* p = (short*)(((long)c + 0x1d8));
        short old = *p;
        *p = old - 1;
        if (cv <= 0) {
            func_ov006_020c1420(c, *(short*)(c + 0x1de), *(int*)(c + 0x1d4));
            return;
        }
    }

    if (!_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 4))
        return;

    _ZN5Sound12PlayBank2_2DEj(0x141);
    if (*(short*)(c + 0x1d8) == *(short*)(c + 0x1da)) {
        V* o = data_ov004_020beb68;
        if (o)
            o->v21();
    }
}
}

// ---- func_ov006_020c1420.cpp ----
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

extern "C" { extern P2 data_ov006_0213ac48; }

extern "C" void func_ov006_020c1420(char *c, short arg1, int arg2)
{
    *(short *)(c + 0x1de) = arg1;
    *(int *)(c + 0x1d4) = arg2;
    if (func_ov006_020c1718((int *)c) && *(int *)(c + 0x7c) != *(int *)(c + 0x234)) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), *(void **)(c + 0x204), 0, 0x40000000, 0x800, 0);
    } else {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), *(void **)(c + 0x214), 0, 0x40000000, 0x800, 0);
    }
    *(P2 *)c = data_ov006_0213ac48;
}
}

// ---- func_ov006_020c11c0.cpp ----
namespace s020c11c0 {
extern "C" {

void func_ov006_020c11c0(char *c)
{
    void *f7c = *(void **)(c + 0x7c);

    if (f7c == *(void **)(c + 0x214)) {
        if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x6c)) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void **)(c + 0x21c), 0, 0, 0x800, 0);
        }
    }

    f7c = *(void **)(c + 0x7c);
    if (f7c == *(void **)(c + 0x224) || f7c == *(void **)(c + 0x22c)) {
        if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x6c)) {
            *(int *)(c + 0x1d4) = 0;
            func_ov006_020c1764(c);
        }
        return;
    }

    if (f7c == *(void **)(c + 0x21c)) {
        if (_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 0)) {
            short cv = *(short *)(c + 0x100 + 0xde);
            short *p = (short *)(((long)c + 0x1de));
            short old = *p;
            *p = old - 1;
            if (cv <= 0) {
                short state = *(short *)(c + 0x1a);
                if (state == 0) {
                    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void **)(c + 0x224), 0, 0x40000000, 0x800, 0);
                    if (*(int *)(c + 0x26c) != 0)
                        return;
                    _ZN5Sound12PlayBank2_2DEj(0x13a);
                    *(int *)(c + 0x26c) = 1;
                    return;
                } else if (state == 2) {
                    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void **)(c + 0x22c), 0, 0x40000000, 0x800, 0);
                    if (*(int *)(c + 0x26c) != 0)
                        return;
                    _ZN5Sound12PlayBank2_2DEj(0x13a);
                    *(int *)(c + 0x26c) = 1;
                    return;
                } else {
                    func_ov006_020c1764(c);
                    return;
                }
            }
        }

        if (*(int *)(c + 0x1d4) == 0) return;
        if (!_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 5)) return;
        _ZN5Sound12PlayBank2_2DEj(0x142);
        {
            short *p = *(short **)(c + 0x1d4);
            *p = (short)(*p + 1);
        }
        return;
    }

    if (f7c == *(void **)(c + 0x204)) {
        if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x6c)) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void **)(c + 0x214), 0, 0x40000000, 0x800, 0);
        }
    }
}
}
}

// ---- func_ov006_020c1164.c ----
namespace s020c1164 {
extern "C" {
struct P2 { int w[2]; };
extern "C" { extern P2 data_ov006_0213acb8; }

void func_ov006_020c1164(int *t, int a1, int a2)
{
    *(short *)((char *)t + 0x1de) = a1;
    t[0x75] = a2;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x1c, (void *)t[0x87], 8, 0, 0x800, 0);
    *(P2 *)t = data_ov006_0213acb8;
}
}
}

// ---- func_ov006_020c0f9c.cpp ----
namespace s020c0f9c {
extern "C" {
}

extern "C" void func_ov006_020c0f9c(void *cc)
{
    char *c = (char*)cc;

    if (*(int*)(c + 0x7c) != *(int*)(c + 0x21c))
        goto other;

    if (_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 0)) {
        short cv = *(short*)(c + 0x100 + 0xde);
        short *p = (short*)(((long)c + 0x1de));
        short old = *p;
        *p = old - 1;
        if (cv <= 0) {
            int a;
            *(int*)(c + 0x1d4) = 0;
            a = *(short*)(c + 0x1a);
            if (a == 0) {
                _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void**)(c + 0x224), 0, 0x40000000, 0x800, 0);
                if (*(int*)(c + 0x26c) != 0)
                    return;
                _ZN5Sound12PlayBank2_2DEj(0x13a);
                *(int*)(c + 0x26c) = 1;
                return;
            } else if (a == 2) {
                _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x1c, *(void**)(c + 0x22c), 0, 0x40000000, 0x800, 0);
                if (*(int*)(c + 0x26c) != 0)
                    return;
                _ZN5Sound12PlayBank2_2DEj(0x13a);
                *(int*)(c + 0x26c) = 1;
                return;
            } else {
                func_ov006_020c1764(c);
                return;
            }
        }
    }

    if (*(int*)(c + 0x1d4) == 0)
        return;
    if (!_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x6c, 5))
        return;
    _ZN5Sound12PlayBank2_2DEj(0x142);
    *(short*)*(char**)(c + 0x1d4) += 1;
    return;

other:
    if (*(int*)(c + 0x7c) == *(int*)(c + 0x224) || *(int*)(c + 0x7c) == *(int*)(c + 0x22c)) {
        if (_ZN15dExtFrameCtrl_c8FinishedEv(c + 0x6c))
            func_ov006_020c1764(c);
    }
}
}

// ---- func_ov006_020c0f0c.c ----
namespace s020c0f0c {
extern "C" {
extern "C" { extern int data_ov006_0213acb0[2]; }
extern "C" { extern int data_ov006_0213aca8[2]; }

int func_ov006_020c0f0c(int *c)
{
    int *p = data_ov006_0213acb0;
    int r = 1;
    int m1 = r;
    int x = *c;
    if (x == *p) {
        if (c[1] == p[1] || x == 0)
            m1 = 0;
    }
    if (m1 != 0) {
        int m2 = 1;
        p = data_ov006_0213aca8;
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

// ---- func_ov006_020c0efc.c ----
namespace s020c0efc {
extern "C" {
/* func_ov006_020c0efc @ 0x20c0efc (ov006) -- veneer: add r0,r0,#0x6c; b dExtFrameCtrl_c::Finished(). */

int func_ov006_020c0efc(void* a) {
    return _ZN15dExtFrameCtrl_c8FinishedEv((char*)a + 0x6c);
}
}
}

// ---- func_ov006_020c0e8c.c ----
namespace s020c0e8c {
extern "C" {
struct P2 { int w[2]; };
extern "C" { extern P2 data_ov006_0213aca0; }

void func_ov006_020c0e8c(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x1c, (void *)t[0x8f], 0, 0x40000000, 0x800, 0);
    _ZN5Model12HideMaterialEii((char *)t + 0x1c, 2, 0);
    _ZN5Model12ShowMaterialEii((char *)t + 0x1c, 1, 0);
    *(P2 *)t = data_ov006_0213aca0;
}
}
}

// ---- func_ov006_020c0df0.cpp ----
namespace s020c0df0 {

extern "C" void func_ov006_020c0df0(char *c)
{
    if (*(int *)(c + 0x268) == 0) {
        Sound::PlayBank2_2D(0x139);
        *(int *)(c + 0x268) = 1;
    }
    if (*(int *)(c + 0x7c) == *(int *)(c + 0x23c) && ((dExtFrameCtrl_c *)(c + 0x6c))->Finished()) {
        ((Model *)(c + 0x1c))->HideMaterial(1, 0);
        ((Model *)(c + 0x1c))->ShowMaterial(2, 0);
        return;
    }
    if (((dExtFrameCtrl_c *)(c + 0x6c))->WillHitFrame(0x14))
        Sound::PlayBank2_2D(0x156);
}
}

// ---- func_ov006_020c0d68.cpp ----
namespace s020c0d68 {
typedef int Fix12;
struct BCA_File;
struct BlendModelAnim { int pad; void SetAnim(BCA_File &f, int a, int b, Fix12 d, unsigned short e); };
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */

extern "C" { extern double data_ov006_0213ac98; }

extern "C" void func_ov006_020c0d68(char *c)
{
    if (*(short *)(c + 0x1a) == 1) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), (void *)*(int *)(c + 0x224), 0, 0x40000000, 0x800, 0);
    } else {
        Sound::PlayBank2_2D(0x13c);
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x1c), (void *)*(int *)(c + 0x25c), 0, 0x40000000, 0x800, 0);
    }
    *(double *)c = data_ov006_0213ac98;
}
}

// ---- func_ov006_020c0ce8.c ----
namespace s020c0ce8 {
extern "C" {
typedef int Fix12;
void func_ov006_020c0ce8(char *c){
    if(!_ZN15dExtFrameCtrl_c8FinishedEv(c+0x6c)) return;
    if(*(int*)(c+0x7c)==*(int*)(c+0x25c)){
        func_ov006_020c1764(c);
        return;
    }
    _ZN5Sound12PlayBank2_2DEj(0x13c);
    *(short*)(c+0x1a)=0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x1c, *(void**)(c+0x25c), 0, 0x40000000, 0x800, 0);
}
}
}

// ---- func_ov006_020c0c80.cpp ----
namespace s020c0c80 {
extern "C" {
struct G2 { int w[2]; };
extern "C" { extern struct G2 data_ov006_0213ac80; }
void func_ov006_020c0c80(void *c)
{
    char *r4 = (char *)c;
    _ZN5Sound12PlayBank2_2DEj(0x13b);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(r4 + 0x1c, *(void **)(r4 + 0x244), 0, 0x40000000, 0x800, 0);
    *(short *)(r4 + 0x1d8) = 5;
    *(struct G2 *)r4 = data_ov006_0213ac80;
}
}
}

// ---- func_ov006_020c0b74.c ----
namespace s020c0b74 {
extern "C" {
typedef struct {
    char _pad[0x1d8];
    short field_1d8;
} Obj;

void func_ov006_020c0b74(char *p) {
    if (*(void **)(p + 0x7c) == *(void **)(p + 0x244) && _ZN15dExtFrameCtrl_c8FinishedEv(p + 0x6c)) {
        _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(p + 0x1c, *(void **)(p + 0x24c), 0, 0, 0x800, 0);
        return;
    }
    if (*(void **)(p + 0x7c) == *(void **)(p + 0x24c) && _ZNK15dExtFrameCtrl_c12WillHitFrameEi(p + 0x6c, 0)) {
        *(short *)(p + 0x1d8) -= 1;
        if (((Obj *)p)->field_1d8 == 0) {
            _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(p + 0x1c, *(void **)(p + 0x254), 0, 0x40000000, 0x800, 0);
            return;
        }
    }
    if (*(void **)(p + 0x7c) == *(void **)(p + 0x254) && _ZN15dExtFrameCtrl_c8FinishedEv(p + 0x6c)) {
        func_ov006_020c1764(p);
    }
}
}
}

// ---- func_ov006_020c0af8.c ----
namespace s020c0af8 {
extern "C" {
extern "C" { extern volatile struct Matrix4x3 data_ov006_0213ad28; }
// @symbol func_ov006_020c0af8
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
void func_ov006_020c0af8(char* c)
{
    struct Matrix4x3 tmp;
    tmp = *(struct Matrix4x3*)&data_ov006_0213ad28;
    *(struct Matrix4x3*)(c + 0x38) = (struct Matrix4x3 &)data_ov006_0213ad28;
    *(struct Matrix4x3*)(c + 0xa8) = tmp;
}
}
}

// ---- func_ov006_020c0aa8.c ----
namespace s020c0aa8 {
extern "C" {
// @symbol func_ov006_020c0aa8
/* recovered: shared common types */
/* func_ov006_020c0aa8 at 0x020c0aa8
 *
 * dCamera_c preset init: sets eye/target vectors and angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of func_ov006_020c225c.
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


void func_ov006_020c0aa8(struct dCamera_c *self)
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

// ---- func_ov006_020c0a48.c ----
namespace s020c0a48 {
extern "C" {
int func_ov006_020c0a48(char *t)
{
    func_02017acc(t, 0x205);
    _ZN13SharedFilePtr9ConstructEj(t + 8, 0x206);
    _ZN13SharedFilePtr9ConstructEj(t + 0x10, 0x207);
    _ZN14BlendModelAnimC1Ev(t + 0x18);
    _ZN17dExtShadowModel_cC1Ev(t + 0x88);
    *(int *)(t + 0xe4) = 0;
    *(short *)(t + 0xf0) = 0;
    *(int *)(t + 0xf4) = 0;
    return (int)t;
}
}
}

// ---- func_ov006_020c09f8.c ----
namespace s020c09f8 {
extern "C" {
int func_ov006_020c09f8(char *t)
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

// ---- func_ov006_020c092c.cpp ----
namespace s020c092c {
// @symbol func_ov006_020c092c
/* recovered: shared common types */
typedef int Fix12i;
struct SharedFilePtr; struct BMD_File; struct BCA_File;
struct ModelBase { int d; };
struct BlendModelAnim { int d; };

extern "C" void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
    BlendModelAnim*, BCA_File&, int, int, Fix12i, unsigned short);



extern "C" { extern Vector3 data_020a0ebc; }
extern "C" { extern Vector3_16 data_020a0edc; }

extern "C" void func_ov006_020c092c(char* thiz)
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
    func_ov006_020c057c(c);
}
}

// ---- func_ov006_020c07e8.cpp ----
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


extern "C" { extern Vector3 data_ov006_0212b890; }
extern "C" { extern "C" int data_ov006_0213ac78[2]; }

extern "C" void func_ov006_020c07e8(C* c)
{
    (c->*(c->pmf))();

    _ZN14BlendModelAnim7AdvanceEv((char*)c + 0x18);

    if (c->a0f4 != 0) {
        Vector3 t;
        t.x = data_ov006_0212b890.x;
        t.y = data_ov006_0212b890.y;
        t.z = data_ov006_0212b890.z;
        short d;
        int ang;
        ang = Vec3_HorzAngle(&c->v0c8, &t);
        d = (short)(ang - c->a0ea);
        if (d < -0x3000) d = -0x3000;
        else if (d > 0x3000) d = 0x3000;
        _Z11UpdateAngleRssis(&c->a0f0, d, 8, 0x200);

        {
            int* p = (int*)(((int)c + 0xb4));
            int* g = data_ov006_0213ac78;
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

// ---- func_ov006_020c07a0.c ----
namespace s020c07a0 {
extern "C" {
void func_ov006_020c07a0(char *t)
{
    func_ov006_020c06dc(t);
    _ZN17dExtShadowModel_c9InitModelEP9Matrix4x35Fix12IiES3_S3_j(t + 0x88, t + 0x34, 0x3c000, 0x14000, 0x3c000, 0xc);
    _ZN5Model6RenderEPK7Vector3(t + 0x18, 0);
}
}
}

// ---- func_ov006_020c06dc.cpp ----
namespace s020c06dc {
// @symbol func_ov006_020c06dc
/* recovered: shared common types */
typedef short s16;




extern "C" { extern Matrix4x3 data_020a0e68; }

struct Base {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void m();  // slot 3 -> offset 0xc
};

extern "C" void func_ov006_020c06dc(char* thiz)
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

// ---- func_ov006_020c057c.cpp ----
namespace s020c057c {
typedef struct Pair { int a, b; } Pair;
extern "C" {
extern int data_ov006_0213acc0[];
extern int data_ov006_0213ac68[];
void func_ov006_020c057c(char* c) {
    int sp8[3];
    int sp14[3];
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x18, (void*)*(int*)(c+0x14), 0, 0, 0x800, 0);
    if (*(int*)(c+0xe4) < 3) {
        *(int *)(((int)c + 0xe4)) += 1;
    } else {
        *(int*)(c+0xe4) = 0;
    }
    *(int*)(c+0xb0) = (int)((char*)data_ov006_0213acc0 + (*(int*)(c+0xe4) << 3));
    *(int*)(c+0xe0) = 0;
    {
        int t = *(int*)(c+0xe0) * 0xc;
        int* arr = (int*)((int*)*(int*)(c+0xb0))[1];
        int* el = (int*)((char*)arr + t);
        *(int*)(c+0xc8) = el[0];
        *(int*)(c+0xcc) = el[1];
        *(int*)(c+0xd0) = el[2];
    }
    *(int *)(((int)c + 0xe0)) += 1;
    Vec3_Sub(sp8, c+0xc8, (char*)((int*)*(int*)(c+0xb0))[1] + *(int*)(c+0xe0) * 0xc);
    *(int*)(c+0xd4) = sp8[0];
    *(int*)(c+0xd8) = sp8[1];
    *(int*)(c+0xdc) = sp8[2];
    if (NormalizeVec3IfNonZero(c+0xd4) == 0) {
        *(int*)(c+0xd4) = 0xb50;
        *(int*)(c+0xd8) = 0xb50;
        *(int*)(c+0xdc) = 0;
    }
    {
        int t = *(int*)(c+0xe0) * 0xc;
        int* arr = (int*)((int*)*(int*)(c+0xb0))[1];
        int* el = (int*)((char*)arr + t);
        sp14[0] = el[0];
        sp14[1] = el[1];
        sp14[2] = el[2];
    }
    *(short*)(c+0xee) = (short)Vec3_HorzAngle(c+0xc8, sp14);
    {
        int a = data_ov006_0213ac68[0];
        int b = data_ov006_0213ac68[1];
        *(int*)(c+0xb4) = b ? a : a;
        *(int*)(c+0xb8) = b;
    }
}
}
}

// ---- func_ov006_020c0364.c ----
namespace s020c0364 {
extern "C" {
typedef short s16;
typedef long long s64;

typedef struct { int x, y, z; } Vector3;


void func_ov006_020c0364(char *c)
{
    char *path;
    char *arr;
    int idx;
    int a, b;
    int t1, t2;
    int r1, r2;

    _Z11UpdateAngleRssis((short *)(c + 0xea), *(short *)(c + 0xee), 8, 0x100);

    a = *(int *)(c + 0xd4);
    if (a < 0)
        a = -a;
    t1 = (int)(((s64)a * 0x1a9 + 0x800) >> 12);

    b = *(int *)(c + 0xdc);
    if (b < 0)
        b = -b;

    idx = *(int *)(c + 0xe0) * 0xc;
    path = *(char **)(c + 0xb0);
    arr = *(char **)(path + 4);
    r1 = _Z14ApproachLinearRiii((int *)(c + 0xc8), *(int *)(arr + idx), t1);

    t2 = (int)(((s64)b * 0x1a9 + 0x800) >> 12);

    idx = *(int *)(c + 0xe0) * 0xc;
    path = *(char **)(c + 0xb0);
    arr = *(char **)(path + 4);
    r2 = _Z14ApproachLinearRiii((int *)(c + 0xd0), *(int *)(arr + idx + 8), t2);

    if (r1 != 0 && r2 != 0) {
        if (*(int *)(c + 0xe0) < *(int *)(*(char **)(c + 0xb0)) - 1) {
            Vector3 dir;
            Vector3 next;
            int *p;

            p = (int *)(c + 0xe0);
            *p = *p + 1;

            path = *(char **)(c + 0xb0);
            idx = *(int *)(c + 0xe0) * 0xc;
            arr = *(char **)(path + 4);
            Vec3_Sub(&dir, (Vector3 *)(c + 0xc8), (Vector3 *)(arr + idx));

            *(int *)(c + 0xd4) = dir.x;
            *(int *)(c + 0xd8) = dir.y;
            *(int *)(c + 0xdc) = dir.z;

            if (NormalizeVec3IfNonZero((Vector3 *)(c + 0xd4)) == 0) {
                *(int *)(c + 0xd4) = 0xb50;
                *(int *)(c + 0xd8) = 0xb50;
                *(int *)(c + 0xdc) = 0;
            }

            idx = *(int *)(c + 0xe0) * 0xc;
            path = *(char **)(c + 0xb0);
            arr = *(char **)(path + 4);
            next.x = *(int *)(arr + idx);
            next.y = *(int *)(arr + idx + 4);
            next.z = *(int *)(arr + idx + 8);

            *(s16 *)(c + 0xee) = Vec3_HorzAngle((const Vector3 *)(c + 0xc8), &next);
            return;
        }

        func_ov006_020c057c(c);
        return;
    }

    if (_Z15ApproachLinear2Rsss((short *)(c + 0xf2), 0, 1) == 0)
        return;

    if (_ZNK15dExtFrameCtrl_c12WillHitFrameEi(c + 0x68, 0) == 0)
        return;

    func_ov006_020c0304((int *)c);
}
}
}

// ---- func_ov006_020c0304.c ----
namespace s020c0304 {
extern "C" {
struct P2 { int w[2]; };
extern "C" { extern P2 data_ov006_0213ac70; }

void func_ov006_020c0304(int *t)
{
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((char *)t + 0x18, (void *)t[3], 8, 0, 0x800, 0);
    *(short *)((char *)t + 0xf2) = 0x78;
    t[0x3d] = 1;
    *(P2 *)((char *)t + 0xb4) = data_ov006_0213ac70;
}
}
}

// ---- func_ov006_020c0264.cpp ----
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
extern "C" { extern P2 data_ov006_0213ac88; }

extern "C" void func_ov006_020c0264(char *c)
{
    if (ApproachLinear2(*(short *)(c + 0xf2), 0, 1) == 0)
        return;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt((BlendModelAnim *)(c + 0x18), *(void **)(c + 0x14), 0, 0, 0x800, 0);
    int r = RandomIntInternal(&data_0209e650);
    int idx = (int)((unsigned int)(r & 0x7fffffff) >> 0x13);
    *(short *)(c + 0xf2) = (short)(((idx * 0x258) >> 12) + 0x258);
    *(int *)(c + 0xf4) = 0;
    *(P2 *)(c + 0xb4) = data_ov006_0213ac88;
}
}

// ---- Camera_UpdateMatrices.c ----
namespace s020c0134 {
extern "C" {
// @symbol Camera_UpdateMatrices
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
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

    _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(
        &self->target, &up, &self->eye, 1, &self->viewMat);

    _Z13CopyToViewMatPK9Matrix4x3(&self->viewMat);

    _ZN8dClipper13Func_020156DCEitii(
        data_0209f43c, 0x1555, self->angle, 0x1000, 0x1388000);
}
}
}

// ---- func_ov006_020bfff8.cpp ----
namespace s020bfff8 {
extern "C" {
extern short data_02082214[];

void func_ov006_020bfff8(char* r4, void* r1, int* r6, int* r5) {
    int out[3];
    MulVec3Mat4x3(r1, r4, out);
    int ang = *(short*)(r4 + 0xb8) >> 4;
    int i = ang * 2;
    int cosv = data_02082214[i];
    int sinv = data_02082214[i + 1];
    int f1 = _ZN4cstd4fdivEii(cosv, sinv);
    int r = (int)(((long long)f1 * (long long)(-out[2]) + 0x800) >> 0xc);
    int f2 = _ZN4cstd4fdivEii(out[1], r);
    int t1 = (int)(((long long)f2 * (long long)0x5f800u + 0x800) >> 0xc);
    r5[0] = -((t1 + (int)0xfffa0800) >> 0xc);
    int g = (int)(((long long)r * (long long)0x1555u + 0x800) >> 0xc);
    int f3 = _ZN4cstd4fdivEii(out[0], g);
    int t3 = (int)(((long long)f3 * (long long)0x7f800u + 0x800) >> 0xc);
    r6[0] = (t3 + 0x7f800) >> 0xc;
}
}
}

// ---- func_ov006_020bfec0.cpp ----
namespace s020bfec0 {
extern "C" {
extern short data_02082214[];

void func_ov006_020bfec0(char* r4, void* r1, short* r5) {
    int out[3];
    MulVec3Mat4x3(r1, r4, out);
    int ang = *(short*)(r4 + 0xb8) >> 4;
    int i = ang * 2;
    int cosv = data_02082214[i];
    int sinv = data_02082214[i + 1];
    int f1 = _ZN4cstd4fdivEii(cosv, sinv);
    int r = (int)(((long long)f1 * (long long)(-out[2]) + 0x800) >> 0xc);
    int f2 = _ZN4cstd4fdivEii(out[1], r);
    int t1 = (int)(((long long)f2 * (long long)0x5f800u + 0x800) >> 0xc);
    r5[1] = -((t1 + (int)0xfffa0800) >> 0xc);
    int g = (int)(((long long)r * (long long)0x1555u + 0x800) >> 0xc);
    int f3 = _ZN4cstd4fdivEii(out[0], g);
    int t3 = (int)(((long long)f3 * (long long)0x7f800u + 0x800) >> 0xc);
    r5[0] = (t3 + 0x7f800) >> 0xc;
}
}
}
