//cpp
/* Production translation unit for ov034/daHanachan_c, the Wiggler.
 * 34 function(s), .text 0x021111a0..0x021136a4.
 *
 * NAME: _ZTS12daHanachan_c is "12daHanachan_c" at ov034 0x02114478; _ZTI at
 * 0x0211445c reads [__si_class_type_info, that string, _ZTI12dEnemyBase_c].
 * The vtable's address point is 0x021144bc; the word before it (0x021144b8)
 * is that _ZTI. The tree previously called the class Wiggler (coined).
 *
 * Six methods and the 28 func_ov034 helpers between them. The run is
 * gap-free and this is the only class in it. daHanachan_c_classInit at
 * 0x021136a4 is the next function and is not part of this translation unit.
 *
 * #pragma defer_codegen off emits .text in source order, so the file is
 * ROM-ascending. One out-of-line destructor is the key function, so this TU
 * emits _ZTV/_ZTI/_ZTS: D1 (0x021111a0), D0 (0x021112b0), then a D2 the
 * cartridge has no home for (manifest: deadstrip).
 */

#pragma defer_codegen off

#include "daHanachan_c.h"
#include "common.h"
#include "decl_Message.h"
#include "types.h"
#include "decl_Player.h"
#include "SharedFilePtr.h"
#include "TextureSequence.h"
#include "Message.h"
#include "Player.h"
#include "dBgCh_Gnd.h"

/* shadow struct 'Vec3D' */
struct Vec3D { int x, y, z; ~Vec3D() {} };

/* shadow struct 'Ent' */
struct Ent {
    int pad;
    void *f;
};



/* shadow struct 'Sub' */
struct Sub { virtual int g0(); virtual int g1(); virtual int g2(); virtual int g3(); virtual int g4(); virtual int g5(void*); };

extern "C" {
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* anim, void* file, int a, int b, unsigned int u);
extern void **data_ov034_02113888[];
extern "C" int DecIfAbove0_Byte(void*);
extern "C" void func_ov034_021125b8(void*, int);
void _Z14ApproachLinearRiii(int *r, int b, int c);
extern void **data_ov034_0211389c[];
extern void **data_ov034_02113860[];
extern "C" int Math_Function_0203b14c(void *base, int a, int b, int c, int d);
extern "C" int data_ov034_021138c4[];
extern void func_0201267c(int, void *);
extern void _ZN6Camera9SetFlag_3Ev(void* cam);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern void _Z14ApproachLinearRsss(short* p, short a, short b);
extern short data_ov034_02113820[];
extern void **data_ov034_021138b0[];
extern void Vec3_Sub(void *out, void *a, void *b);
extern int LenVec3(void *v);
extern int Vec3_HorzDist(void *a, void *b);
extern int RandomIntInternal(int *seed);
extern void func_ov034_02112688(char *c);
extern int data_0209e650;
extern int data_ov034_02114488[];
void _ZN5Sound22LoadAndSetMusic_Layer3Ej(unsigned);
void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern char data_020a0e68[];
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void MulVec3Mat4x3(void *in, void *m, void *out);
extern "C" void Vec3_Add(void *out, void *a, void *b);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, struct Vector3 *v, unsigned int b, int c, unsigned int d, unsigned int e, unsigned int f);
extern int NormalizeVec3IfNonZero(void *v);
extern void Vec3_MulScalarInPlace(void *v, int s);
extern void _ZN9dBgCh_GndC1Ev(void *self);
extern void _ZN9dBgCh_GndD1Ev(void *self);
extern SharedFilePtr *data_ov034_02113838[];
extern SharedFilePtr *data_ov034_0211384c[];
extern void **data_ov034_02113874[];
extern void func_ov034_02112604(void *c);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(s32 x, s32 y, s32 z);
extern void func_ov034_02112874(void *c);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
u32 a, u32 b, s32 x, s32 y, s32 z, const void *v, void *cb);
extern void func_ov034_021129ec(void *c);
extern int func_ov002_020c51d0(void *, void *);
extern unsigned char data_ov034_0211433c[];
extern s32 data_ov034_021138d8[];
extern void func_020092c4(void *arg0, void *out, void *target);
extern u32 data_ov034_02113828[];
extern s16 data_02082214[];
extern void *data_0209f318;
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *ts, void *btp, int i, int fix, u32 j);
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *actor, void *pos, int fix, u32 a, u32 b, u32 cc);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int fix1, int fix2, void *v, int t);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN12daHanachan_cD1Ev, 0x021111a0, size 0x110 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daHanachan_cD1Ev
// @symbol _ZN12daHanachan_cD0Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Eight array cleanups and the chain into dEnemyBase_c, reverse declaration order out
 * of daHanachan_c.h. Five body segments, so five of everything: a ModelAnim, a
 * MaterialChanger and a TextureSequence each, two Vector3 arrays and a
 * Vector3s array of per-segment values, and two sets of collision cylinders.
 *
 * The Vector3s array at 0x444 is what named func_02011508.
 *
 * D0 is the deleting destructor: it destroys through this class and its
 * bases, which is why more than one vptr store appears, then frees through
 * an inline operator delete, which is why nothing here mentions a heap.
 */
daHanachan_c::~daHanachan_c()
{
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov034_021113d4, 0x021113d4, size 0x14c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_021113d4
extern "C" {  /* .c-derived member: C linkage for the whole block */

void func_ov034_021113d4(char *thiz) {
    int i;
    int k;
    char *anim;
    anim = thiz + 0x110;
    anim = anim + 0xc8;
    for (i = 2, k = 6; i < 5; i++, k += 3, anim += 0x64) {
        if (*(unsigned char *)(thiz + 0x8da) == k - 3)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_02113888[i][1], 0x40000000, 0x1000, 0);
    }
    {
        int idx; char *fin; char *an;
        fin = thiz + 0x160; idx = 0; an = thiz + 0x110;
        for (; idx < 5; idx++, fin += 0x64, an += 0x64) {
            if (((Animation *)fin)->Finished())
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(an, data_ov034_02113874[idx][1], 0, 0x1000, 0);
        }
    }
    {
int flag; int i; char *p; int zero;
p = thiz;
flag = 1;
zero = 0;
i = 0;

        for (; i < 5; i++, p += 0x64) {
            if (*(int *)(p + 0x170) != (int)data_ov034_02113874[i][1])
                flag = zero;
        }
        if (flag == 1)
            func_ov034_021125b8(thiz, 4);
    }
    {
        unsigned char *t = (unsigned char *)(void *)(thiz + 0x8da);
        *t = (unsigned char)(*t + 1);
    }
    if (*(unsigned char *)(thiz + 0x8da) > 0xc)
        *(unsigned char *)(thiz + 0x8da) = 0xd;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov034_02111520, 0x02111520, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02111520
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02111520(char *p) {
    *(char*)(p+0x8da)=0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0x110, data_ov034_02113888[0][1], 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0x174, data_ov034_02113888[1][1], 0x40000000, 0x1000, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov034_02111588, 0x02111588, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02111588
extern "C" void func_ov034_02111588(char *c){
  if(DecIfAbove0_Byte(c+0x8da)) return;
  func_ov034_021125b8(c, 0xa);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov034_021115c0, 0x021115c0, size 0xc */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_021115c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_021115c0(char *p)
{
    p[2266] = 90;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov034_021115cc, 0x021115cc, size 0x154 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_021115cc
extern "C" void func_ov034_021115cc(char *self) {
    int i;
    int k;
    char *anim;

    _Z14ApproachLinearRiii((int *)(self + 0x98), 0, 0x1000);

    anim = self + 0x110;
    anim = anim + 0xc8;
    for (i = 2, k = 6; i < 5; i++, k += 3, anim += 0x64) {
        if (*(unsigned char *)(self + 0x8da) == k - 3)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_0211389c[i][1], 0x40000000, 0x1000, 0);
    }

    if (((Animation *)(self + 0x2f0))->Finished()) {
{
int i; char *p;
p = self + 0x110;
i = 0;

        for (; i < 5; i++, p += 0x64) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p, data_ov034_02113860[i][1], 0, 0x1000, 0);
        }
        }

    }

    {
        int flag; int i; char *p; int zero;
        p = self;
        flag = 1;
        zero = 0;
        i = 0;
        for (; i < 5; i++, p += 0x64) {
            if (*(int *)(p + 0x170) != (int)data_ov034_02113860[i][1])
                flag = zero;
        }
        if (flag == 1)
            func_ov034_021125b8(self, 9);
    }

    {
        unsigned char *t = (unsigned char *)(void *)(self + 0x8da);
        *t = (unsigned char)(*t + 1);
    }
    if (*(unsigned char *)(self + 0x8da) > 0xc)
        *(unsigned char *)(self + 0x8da) = 0xd;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov034_02111720, 0x02111720, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02111720
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02111720(char *p) {
    *(char*)(p+0x8da)=0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0x110, data_ov034_0211389c[0][1], 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0x174, data_ov034_0211389c[1][1], 0x40000000, 0x1000, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov034_02111788, 0x02111788, size 0x1ec */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02111788
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" void func_ov034_02111788(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    Vec3D starPos;
    Vector3 v;
    int a, b;
    int i;

    if (DecIfAbove0_Byte(c + 0x8da) == 0) {
        unsigned int idx = *(unsigned char *)(c + 0x8dd);
        if (idx < 5) {
            Vector3 *vp = (Vector3 *)(c + 0x3cc + idx * 0xc);
            v.x = vp->x;
            v.y = vp->y;
            v.z = vp->z;
            ((dActor_c *)c)->PoofDustAt(v);
            (*(unsigned char *)(((int)c + 0x8dd)))++;
            *(unsigned char *)(c + 0x8da) = 5;
        }
    }

    {
        int *p8 = (int *)c;
        int *p7 = (int *)(c + 0x464);
        int *p6 = (int *)(c + 0x408);
        for (i = 0; i < 5; i++) {
            if (i > (int)*(unsigned char *)(c + 0x8dd)) break;
            p8[0x104] = p8[0x102];
            p8[0x103] = p8[0x104];
            a = Math_Function_0203b14c(p7, data_ov034_021138c4[i] >> 1, 0x800, 0x20000, 0x200) & 0xff;
            b = Math_Function_0203b14c(p6, 0x800, 0x800, 0x2000, 0x200) & 0xff;
            p8 = (int *)((char *)p8 + 0xc);
            p7 = p7 + 1;
            p6 = (int *)((char *)p6 + 0xc);
        }
    }

    if (a != 0) return;
    if (b != 0) return;

    if (*(unsigned char *)(c + 0x8dc) < 5) {
        (*(unsigned char *)(((int)c + 0x8dc)))++;
        return;
    }

    {
        int px = *(int *)(c + 0x5c);
        int pz = *(int *)(c + 0x64);
        int py = *(int *)(c + 0x60) + 0x64000;
        starPos.x = px;
        starPos.y = py;
        starPos.z = pz;
    }
    ((dActor_c *)c)->UntrackAndSpawnStar(*(signed char *)(c + 0x8e3), *(unsigned char *)(c + 0x8e2), *(const Vector3 *)(c + 0x5c), 4);
    _ZN5Sound22StopLoadedMusic_Layer3Ev();
    func_ov034_021125b8(c, 3);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov034_02111974, 0x02111974, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02111974
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02111974(char *r0) {
    r0[0x8dc] = 0;
    r0[0x8dd] = 0;
    r0[0x8e0] = 1;
    r0[0x8da] = 5;
    func_0201267c(287, r0 + 0x74);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov034_021119ac, 0x021119ac, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_021119ac
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_021119ac(int c){
  _Z14ApproachLinearRiii((int*)((char*)c+0x98),0,0x1000);
  if(((Player *)((void *)*(int*)((char*)c+0x8c8)))->GetTalkState()!=2) return;
  if(*(unsigned char*)((char*)c+0x8db)>1){
    func_ov034_021125b8((void *)c,3);
    return;
  }
  func_ov034_021125b8((void *)c,7);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov034_02111a0c, 0x02111a0c, size 0x58 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02111a0c
extern "C" {
struct BCA{int pad; void* f; };
void func_ov034_02111a0c(char* c){
  int i;
  char* p=c+0x110;
  for(i=0;i<5;i++,p+=0x64){
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p, data_ov034_02113860[i][1], 0, 0x1000, 0);
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov034_02111a64, 0x02111a64, size 0x14c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02111a64
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02111a64(char* c)
{
    struct Vector3 st;
    char* talk;

    _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x1000);
    *(short*)(c + 0x8d8) = (short)((dActor_c *)c)->HorzAngleToCPlayer();

    talk = *(char**)(c + 0x8c8);
    st.x = *(int*)(c + 0x3cc);
    st.y = *(int*)(c + 0x3d0);
    st.z = *(int*)(c + 0x3d4);
    st.y = st.y + 0x8c000;
    if (!((Player *)talk)->StartTalk(*(fBase_c *)c, 1))
        return;

    _ZN6Camera9SetFlag_3Ev(data_0209f318);
    *(unsigned char*)(c + 0x8e1) = 1;

    _Z14ApproachLinearRsss((short*)(*(char**)(c + 0x8c8) + 0x8e),
        Vec3_HorzAngle(c + 0x3cc, *(char**)(c + 0x8c8) + 0x5c), 0x100);

    func_ov002_020c51d0(talk, &st);

    if (!((Animation *)(c + 0x160))->Finished())
        return;

    {
        unsigned int idx = (4 - *(unsigned char*)(c + 0x8db)) & 0xff;
        if (idx >= 4)
            idx = 0;
        if (!((Player *)talk)->ShowMessage(*(fBase_c *)c, data_ov034_02113820[idx], &st, 1, 0))
            return;
    }

    func_0201267c(0x11b, c + 0x74);
    func_ov034_021125b8(c, 6);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov034_02111bb0, 0x02111bb0, size 0x98 */
/* -------------------------------------------------------------------------- */
#pragma opt_strength_reduction off
// @symbol func_ov034_02111bb0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02111bb0(char *p) {
    int i;
    char *anim = p + 0x110;
    *(int*)(p+0x8e4) = 0;
    for (i = 0; i < 5; i++) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, ((void**)data_ov034_021138b0[i])[1], 0x40000000, 0x1000, 0);
        int *f = (int*)(((int)(p + (i<<6)) + 0x490));
        *f |= 4;
        anim += 0x64;
    }
    if (*(unsigned char*)(p+0x8db) > 1) {
        unsigned char *q = (unsigned char*)(((int)p + 0x8db));
        *q = *q - 1;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov034_02111c48, 0x02111c48, size 0x204 */
/* -------------------------------------------------------------------------- */
#pragma opt_strength_reduction on
// @symbol func_ov034_02111c48
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02111c48(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    int len;
    int r4;
    int v[3];

    Vec3_Sub(v, c + 0x5c, c + 0x8cc);
    len = LenVec3(v);

    if (*(unsigned char *)(c + 0x8e0) != 0 && *(unsigned char *)(c + 0x8df) == 0) {
        void *cp = ((dActor_c *)c)->ClosestPlayer();
        if (Vec3_HorzDist(c + 0x5c, (char *)cp + 0x5c) < 0xc8000) {
            *(unsigned char *)(c + 0x8df) = 1;
            *(int *)(c + 0x9c) = -0x1000;
            *(int *)(c + 0xa0) = -0x3c000;
            *(int *)(c + 0xa8) = 0;
        }
    }

    if (DecIfAbove0_Byte((unsigned char *)(c + 0x8da)) == 0) {
        r4 = RandomIntInternal(&data_0209e650);
        {
            unsigned char db = *(unsigned char *)(c + 0x8db);
            unsigned int hi;
            if (db <= 3 || (((hi = (unsigned int)r4 >> 0x1b)) & 1) == 0) {
                *(short *)(c + 0x800 + 0xd8) = ((dActor_c *)c)->HorzAngleToCPlayer();
            } else {
                *(short *)(c + 0x800 + 0xd8) = (short)((hi & 0xf) << 0xc);
            }
        }
        *(unsigned char *)(c + 0x8da) = (unsigned char)((unsigned int)(r4 + 0x1e) >> 0x1b);
        *(unsigned char *)(c + 0x8da) =
            (unsigned char)(*(unsigned char *)(c + 0x8da) - (4 - *(unsigned char *)(c + 0x8db)) * 0x1e);
        if (*(unsigned char *)(c + 0x8da) >= 0x40)
            *(unsigned char *)(c + 0x8da) = 0;
    }

    if (((dBgCh_Actr *)(c + 0x708))->IsOnWall() != 0 || len > 0x5dc000) {
        r4 = RandomIntInternal(&data_0209e650);
        *(short *)(c + 0x800 + 0xd8) = Vec3_HorzAngle(c + 0x5c, c + 0x8cc);
        *(unsigned char *)(c + 0x8da) = (unsigned char)((unsigned int)(r4 + 0x1e) >> 0x1b);
        *(unsigned char *)(c + 0x8da) =
            (unsigned char)(*(unsigned char *)(c + 0x8da) - (4 - *(unsigned char *)(c + 0x8db)) * 0x1e);
        if (*(unsigned char *)(c + 0x8da) >= 0x40)
            *(unsigned char *)(c + 0x8da) = 0;
    }

    {
        unsigned int idx = (unsigned int)((4 - *(unsigned char *)(c + 0x8db)) & 0xff);
        if (idx >= 4)
            idx = 0;
        _Z14ApproachLinearRiii((int *)(c + 0x98), data_ov034_02114488[idx], 0x1000);
    }

    if (*(unsigned char *)(c + 0x8e0) != 0)
        return;
    func_ov034_02112688((char *)c);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov034_02111e4c, 0x02111e4c, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02111e4c
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02111e4c(char* c) {
    *(int*)(c + 0x8c8) = 0;
    if (*(unsigned char*)(c + 0x8e0) != 0) {
        *(unsigned char*)(c + 0x8da) = 0x96;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov034_02111e68, 0x02111e68, size 0x1b8 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02111e68
extern "C" {  /* .c-derived member: C linkage for the whole block */

void func_ov034_02111e68(char *c)
{
  int i;
  int k;
  char *anim;
  unsigned st;
  void *player;

  anim = c + 0x110;
  anim = anim + 0xc8;
  for (i = 2, k = 6; i < 5; i++, k += 3, anim += 0x64)
  {
    if ((*((unsigned char *) (c + 0x8da))) == (k - 3))
    {
      _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_02113888[i][1], 0x40000000, 0x1000, 0);
    }
  }

  {
    int idx;
    char *fin;
    char *an;
    fin = c + 0x160;
    idx = 0;
    an = c;
    an = an + 0x110;
    for (; idx < 5; idx++, fin += 0x64, an += 0x64)
    {
      if (((Animation *)fin)->Finished())
      {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(an, data_ov034_02113874[idx][1], 0, 0x1000, 0);
      }
    }
  }

  player = *(void **)(c + 0x8c8);
  {
    int flag;
    int i;
    char *p;
    int zero;
    p = c;
    flag = 1;
    zero = 0;
    i = 0;
    for (; i < 5; i++, p += 0x64)
    {
      if ((*((int *) (p + 0x170))) != ((int) data_ov034_02113874[i][1]))
      {
        flag = zero;
      }
    }

    if (flag == 1)
    {
      if (((Player *)player)->HasFinishedTalking())
      {
        st = *((unsigned char *) (c + 0x8db));
        if (st < 4)
        {
          if (st > 1)
          {
            int t = (int) (st - 2);
            int m = t * 0xa;
            *((int *) (c + 0xa8)) = 0x41000 - (m << 0xc);
            *((int *) (c + 0x9c)) = -0x4000;
          }
        }
        (*(int *)(((int)data_0209f318 + 0x154))) &= ~8;
        *((unsigned char *) (c + 0x8e1)) = 0;
        func_ov034_021125b8(c, 4);
      }
    }
  }
  {
    unsigned char *t = (unsigned char *) ((void *) ((unsigned long long) ((unsigned) (c + 0x8da))));
    *t = (unsigned char) ((*t) + 1);
  }
  if ((*((unsigned char *) (c + 0x8da))) > 0xc)
  {
    *((unsigned char *) (c + 0x8da)) = 0xd;
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov034_02112020, 0x02112020, size 0x8c */
/* -------------------------------------------------------------------------- */
#pragma opt_strength_reduction off
// @symbol func_ov034_02112020
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02112020(char *c)
{
    int i;
    for (i = 0; i < 5; i++) {
        int *p = (int *)(void *)(c + (i << 6) + 0x490);
        *p &= ~4;
    }
    *(unsigned char *)(c + 0x8da) = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov034_02113888[0][1], 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x174, data_ov034_02113888[1][1], 0x40000000, 0x1000, 0);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov034_021120ac, 0x021120ac, size 0x1c4 */
/* -------------------------------------------------------------------------- */
#pragma opt_strength_reduction on
// @symbol func_ov034_021120ac
extern "C" void func_ov034_021120ac(char *c) {
    int i, k;
    char *anim;
    void *player;

    _Z14ApproachLinearRiii((int *)(c + 0x98), 0, 0x1000);

    if (*(unsigned char *)(c + 0x8da) == 0) {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, data_ov034_0211389c[0][1], 0x40000000, 0x1000, 0);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x174, data_ov034_0211389c[1][1], 0x40000000, 0x1000, 0);
    }

    anim = c + 0x110;
    anim = anim + 0xc8;
    for (i = 2, k = 6; i < 5; i++, k += 3, anim += 0x64) {
        if (*(unsigned char *)(c + 0x8da) == k - 3)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(anim, data_ov034_0211389c[i][1], 0x40000000, 0x1000, 0);
    }

    if (((Animation *)(c + 0x2f0))->Finished()) {
{
int i; char *p;
p = c + 0x110;
i = 0;
        for (; i < 5; i++, p += 0x64)
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p, data_ov034_02113860[i][1], 0, 0x1000, 0);
}
    }

    player = *(void **)(c + 0x8c8);

    {
        int flag; int i; char *p; int zero;
        p = c;
        flag = 1;
        zero = 0;
        i = 0;
        for (; i < 5; i++, p += 0x64) {
            if (*(int *)(p + 0x170) != (int)data_ov034_02113860[i][1])
                flag = zero;
        }
        if (flag == 1) {
            if (((Player *)player)->GetTalkState() == 2) {
                _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
                Message::EndTalk();
                func_ov034_021125b8(c, 3);
            }
        }
    }

    {
        unsigned char *t = (unsigned char *)(void *)(c + 0x8da);
        *t = (unsigned char)(*t + 1);
    }
    if (*(unsigned char *)(c + 0x8da) > 0xc)
        *(unsigned char *)(c + 0x8da) = 0xd;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov034_02112270, 0x02112270, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02112270
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02112270(char *p)
{
    *(char *)(p + 0x8da) = 0;
    *(char *)(p + 0x8de) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov034_02112284, 0x02112284, size 0xac */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02112284
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02112284(char* c) {
  struct Vector3 v;
  void* self = *(void**)(c+0x8c8);
  v.x = *(int*)(c+0x3cc);
  v.y = *(int*)(c+0x3d0);
  v.z = *(int*)(c+0x3d4);
  v.y = v.y + 0x28000;
  if (((Player *)self)->ShowMessage(*(fBase_c *)c, *(s16*)data_ov034_02113820, &v, 1, 0) == 0) return;
  _ZN7Message11PrepareTalkEv();
  _ZN6Camera9SetFlag_3Ev(data_0209f318);
  *(unsigned char*)(c+0x8e1) = 1;
  func_0201267c(0x11b, c+0x74);
  func_ov034_021125b8(c, 2);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov034_02112330, 0x02112330, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02112330
extern "C" {
void func_ov034_02112330(char* c){
  *(int*)(c+0x8c8) = (int)((dActor_c *)c)->ClosestPlayer();
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov034_02112348, 0x02112348, size 0x13c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02112348
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02112348(char* c)
{
    struct Vector3 v;
    int len;
    char* player;

    Vec3_Sub(&v, c + 0x5c, c + 0x8cc);
    len = LenVec3(&v);
    player = (char *)((dActor_c *)c)->ClosestPlayer();
    if (*(int*)(player + 0x60) > *(int*)(c + 0x60) + 0x12c000) {
        func_ov034_021125b8(c, 1);
        return;
    }

    if (DecIfAbove0_Byte((unsigned char*)(c + 0x8da)) == 0) {
        int r = (unsigned)RandomIntInternal(&data_0209e650) >> 0x1b;
        if (r & 1) {
            *(short*)(c + 0x8d8) = (short)((r & 0xf) << 0xc);
        } else {
            *(short*)(c + 0x8d8) = ((dActor_c *)c)->HorzAngleToCPlayer();
        }
        *(unsigned char*)(c + 0x8da) = data_ov034_0211433c[*(unsigned char*)(c + 0x8db)];
    }

    if (((dBgCh_Actr *)(c + 0x708))->IsOnWall() || len > 0x5dc000) {
        *(short*)(c + 0x8d8) = Vec3_HorzAngle(c + 0x5c, c + 0x8cc);
        *(unsigned char*)(c + 0x8da) = data_ov034_0211433c[*(unsigned char*)(c + 0x8db)];
    }
    _Z14ApproachLinearRiii((int*)(c + 0x98), data_ov034_02114488[0], 0x1000);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov034_02112484, 0x02112484, size 0x134 */
/* -------------------------------------------------------------------------- */
#pragma opt_strength_reduction off
// @symbol func_ov034_02112484
/* recovered: shared common types */
extern "C" void func_ov034_02112484(char *sl)
{
    int sb;
    char *r8;
    char *r7;
    int zero;
    struct Vector3 in;
    struct Vector3 out;
    struct Vector3 sum;

    sb = 0;
    *(int *)(sl + 0x8c8) = sb;
    *(int *)(sl + 0x98) = data_ov034_02114488[0];
    r8 = sl;
    r7 = sl;
    zero = sb;
    for (; sb < 5; sb++, r8 += 0xc, r7 += 6) {
        if (sb == 0) {
            *(int *)(r8 + 0x3cc) = *(int *)(sl + 0x5c);
            *(int *)(r8 + 0x3d0) = *(int *)(sl + 0x60);
            *(int *)(r8 + 0x3d4) = *(int *)(sl + 0x64);
            {
                short ang = (short)((dActor_c *)sl)->HorzAngleToCPlayer();
                *(short *)(r7 + 0x446) = ang;
                *(short *)(sl + 0x94) = *(short *)(r7 + 0x446);
            }
        } else {
            int r6;
            int zval;
            char *angbase;
            zval = -*(int *)(sl + (sb << 2) + 0x464);
            angbase = sl + 0x400;
            in.x = zero;
            in.y = zero;
            out.x = zero;
            out.y = zero;
            out.z = zero;
            in.z = zval;
            {
                short ang = *(short *)(angbase + 0x46);
                Matrix4x3_FromRotationY(data_020a0e68, (short)(ang + 0x200));
            }
            MulVec3Mat4x3(&in, data_020a0e68, &out);
            r6 = sb - 1;
            Vec3_Add(&sum, (struct Vector3 *)(sl + 0x3cc + r6 * 0xc), &out);
            *(int *)(r8 + 0x3cc) = sum.x;
            *(int *)(r8 + 0x3d0) = sum.y;
            *(int *)(r8 + 0x3d4) = sum.z;
            *(short *)(r7 + 0x446) = *(short *)(sl + r6 * 6 + 0x446);
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov034_021125b8, 0x021125b8, size 0x4c */
/* -------------------------------------------------------------------------- */
#pragma opt_strength_reduction on
// @symbol func_ov034_021125b8
struct C125b8; typedef void (C125b8::*PMF125b8)();
struct Entry125b8 { PMF125b8 pmf; char pad[12]; };
struct C125b8 { char pad[0x8c4]; int idx; };
extern "C" void func_ov034_021125b8(void *raw, int i) {
  extern char data_ov034_02114538[];
  C125b8 *c = (C125b8 *)raw;
  c->idx = i;
  int j = c->idx;
  (c->*((Entry125b8 *)data_ov034_02114538)[j].pmf)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov034_02112604, 0x02112604, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02112604
struct C12604; typedef void (C12604::*PMF12604)();
struct Entry12604 { char pad[8]; PMF12604 pmf; char tail[20 - 8 - sizeof(PMF12604)]; };
struct C12604 { char pad[0x8c4]; int idx; };
extern "C" void func_ov034_02112604(void *raw) {
  extern char data_ov034_02114538[];
  C12604 *c = (C12604 *)raw;
  int j = c->idx;
  (c->*((Entry12604 *)data_ov034_02114538)[j].pmf)();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov034_02112650, 0x02112650, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov034_02112650
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov034_02112650(char *a, char *bp, char *cp)
{
    int *b = (int *)bp;
    int *c = (int *)cp;
    volatile int pad[4];
    int y = b[0x10 / 4];
    int x = c[0xa8 / 4];
    (void)&pad;
    if (x < 0 && y < 0 && y > x)
        return 1;
    return 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov034_02112688, 0x02112688, size 0x1ec */
/* -------------------------------------------------------------------------- */
#pragma opt_strength_reduction off
// @symbol func_ov034_02112688
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02112688(char *sl)
{
    int off; int zero; int i; char *a; char *p; int one; int two; int cc; int eight;
    volatile struct Vector3 v0; struct Vector3 v; struct Vector3 hv;
    off = 0x478; p = sl + off; i = 0; zero = 0; one = 1; two = 2; cc = 0xc000; eight = 8;
    for (; i < 5; i++, p += 0x40) {
        unsigned int id; int flag; u16 type; struct Vector3 *src;
        id = *(unsigned int *)(sl + (i << 6) + 0x49c);
        if (id == 0) continue;
        a = (char *)dActor_c::FindWithID(id);
        if (a == 0) continue;
        type = *(u16 *)(a + 0xc);
        flag = (type == 0xbf) ? one : zero;
        if (flag == 0) continue;
        src = (struct Vector3 *)(a + 0x5c);
        v0.x = src->x; v0.y = src->y; v0.z = src->z;
        if (func_ov034_02112650(sl, p, a) != 0) {
            _ZN6Player6BounceE5Fix12IiE(a, 0x28000);
            func_0201267c(0x11c, sl + 0x74);
            if ((unsigned int)i > 1) { *(u8 *)(sl + i + 0x702) = 0x1e; return; }
            *(u8 *)(sl + 0x702) = 0x1e;
            *(char **)(sl + 0x8c8) = a;
            func_ov034_021125b8(sl, 5);
            return;
        }
        if (i == 0) {
            v.x = *(int *)(sl + 0x5c); v.y = *(int *)(sl + 0x60); v.z = *(int *)(sl + 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &v, two, cc, one, zero, one);
            func_ov034_021125b8(sl, eight);
        }
    }
    {
        int z2; int j; int o2; int z3;
        z2 = 0;
        z3 = z2;
        j = 0;
        o2 = 1;
        for (; j < 5; j++) {
            unsigned int id; int flag; u16 type;
            id = *(unsigned int *)(sl + (j << 6) + 0x5dc);
            if (id == 0) continue;
            a = (char *)dActor_c::FindWithID(id);
            if (a == 0) continue;
            type = *(u16 *)(a + 0xc);
            flag = (type == 0xbf) ? o2 : z2;
            if (flag == 0) continue;
            hv.x = *(int *)(sl + 0x5c); hv.y = *(int *)(sl + 0x60); hv.z = *(int *)(sl + 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(a, &hv, z3, 0x5000, o2, z3, o2);
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov034_02112874, 0x02112874, size 0x178 */
/* -------------------------------------------------------------------------- */
#pragma opt_strength_reduction on
#pragma opt_strength_reduction off
#pragma opt_common_subs off
// @symbol func_ov034_02112874
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_02112874(void *raw)
{
    char *sl = (char *)raw;
    int sb;
    char *r8;
    char *r7;
    char *r6;
    int zero;
    Vector3 diff;
    Vector3 castPos;
    Vector3 sum;
    char ray[0x54];

    r8 = (char *)(sl + 0x3cc) + 0xc;
    r7 = sl + 6;
    r6 = sl + 0xc;
    sb = 1;
    zero = 0;

    for (; sb < 5; sb++, r8 += 0xc, r7 += 6, r6 += 0xc) {
        int off = (sb - 1) * 0xc;
        Vec3_Sub(&diff, r8, (char *)(sl + 0x3cc) + off);
        if (NormalizeVec3IfNonZero(&diff) != 0) {
            Vec3_MulScalarInPlace(&diff, *(int *)(sl + (sb << 2) + 0x464));
        }
        {
            char *prev = (char *)(sl + 0x3cc) + off;
            short ang = Vec3_HorzAngle(r8, prev);
            *(short *)(r7 + 0x446) = ang;
            Vec3_Add(&sum, prev, &diff);
        }

        *(int *)(r6 + 0x3cc) = sum.x;
        *(int *)(r6 + 0x3d0) = sum.y;
        *(int *)(r6 + 0x3d4) = sum.z;
        *(int *)(sl + (sb << 6) + 0x4ac) = *(int *)(r6 + 0x3cc);
        *(int *)(sl + (sb << 6) + 0x4b0) = *(int *)(r6 + 0x3d0);
        *(int *)(sl + (sb << 6) + 0x4b4) = *(int *)(r6 + 0x3d4);
        *(int *)(sl + (sb << 6) + 0x5ec) = *(int *)(r6 + 0x3cc);
        *(int *)(sl + (sb << 6) + 0x5f0) = *(int *)(r6 + 0x3d0);
        *(int *)(sl + (sb << 6) + 0x5f4) = *(int *)(r6 + 0x3d4);

        if (*(unsigned char *)(sl + 0x8df) == 0) {
            int py;
            _ZN9dBgCh_GndC1Ev(ray);
            castPos.x = *(int *)(r6 + 0x3cc);
            py = *(int *)(r6 + 0x3d0);
            castPos.y = py;
            castPos.z = *(int *)(r6 + 0x3d4);
            castPos.y = py + 0x3c000;
            ((dBgCh_Gnd *)ray)->SetObjAndPos(castPos, (dActor_c *)zero);
            if (((dBgCh_Gnd *)ray)->dBgCh_Gnd::DetectClsn() != 0) {
                int clY = *(int *)(ray + 0x44);
                if (*(int *)(r6 + 0x3d0) <= clY)
                    *(int *)(r6 + 0x3d0) = clY;
            }
            _ZN9dBgCh_GndD1Ev(ray);
        }
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov034_021129ec, 0x021129ec, size 0x70 */
/* -------------------------------------------------------------------------- */
#pragma opt_common_subs on
#pragma opt_strength_reduction on
// @symbol func_ov034_021129ec
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov034_021129ec(void *raw)
{
    char *t = (char *)raw;
    int i;
    char *r8 = t;
    char *r6 = t + 0x110;
    char *r5 = t;
    char *r4 = t;
    for (i = 0; i < 5; i++) {
        Matrix4x3_FromRotationY(r6 + 0x1c, *(short *)(r8 + 0x446));
        *(int *)(r4 + 0x150) = *(int *)(r5 + 0x3cc) >> 3;
        *(int *)(r4 + 0x154) = *(int *)(r5 + 0x3d0) >> 3;
        *(int *)(r4 + 0x158) = *(int *)(r5 + 0x3d4) >> 3;
        r8 += 6;
        r6 += 0x64;
        r5 += 0xc;
        r4 += 0x64;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN12daHanachan_c16CleanupResourcesEv, 0x02112a5c, size 0x98 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daHanachan_c16CleanupResourcesEv
/* _ZN12daHanachan_c16CleanupResourcesEv at 0x02112a5c
 *
 * Releases all five entries of each of the seven SharedFilePtr tables this
 * actor pulled, then returns 1.
 */
int daHanachan_c::CleanupResources()
{
    int i;
    for (i = 0; i < 5; i++) {
        data_ov034_02113838[i]->Release();
        data_ov034_0211384c[i]->Release();
        ((SharedFilePtr *)data_ov034_02113860[i])->Release();
        ((SharedFilePtr *)data_ov034_02113874[i])->Release();
        ((SharedFilePtr *)data_ov034_02113888[i])->Release();
        ((SharedFilePtr *)data_ov034_0211389c[i])->Release();
        ((SharedFilePtr *)data_ov034_021138b0[i])->Release();
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN12daHanachan_c6RenderEv, 0x02112af4, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daHanachan_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daHanachan_c::Render()
{
  int i = 0;
  char* p6 = ((char*)this)+0x110;
  char* p5 = ((char*)this)+0x368;
  char* p4 = ((char*)this)+0x408;
  for(;i<5;i++){
    ((TextureSequence *)p5)->TextureSequence::Update(*(ModelComponents *)(p6+8));
    ((Sub*)p6)->g5(p4);
    p6 += 0x64;
    p5 += 0x14;
    p4 += 0xc;
  }
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- _ZN12daHanachan_c8BehaviorEv, 0x02112b5c, size 0x6e0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daHanachan_c8BehaviorEv
/* recovered: real C++ method against include/daHanachan_c.h; the segment arrays and the
   0x6f8/0x8c4 tail fields are header members now (unk_6f8, unk_702, unk_8d0..unk_8e4).

   daHanachan_c::Behavior -- vtable slot 6, ov034 0x02112b5c, 0x6e0 bytes.

   Rewritten from the cartridge disassembly rather than from the flat-offset draft
   that sat here before. Credit for that draft stays with it: it was banked as a
   earlier unmatched draft by lunavyqo, and its structure, extern set and frame layout are
   what made this address readable enough to attack. Two things the flat draft had
   to force by hand fall out of the real shape: strength reduction is left ON and
   the five induction pointers of the segment loop (mTextureSequences[i],
   mModelAnims[i], unk_408[i].y as a store base and as a call argument) colour
   r7/r6/r5/r4 with r8 as the counter by themselves, and the second loop's counter
   gets its own block-scoped `j` so it colours r6 above the two collider pointers.

   The face-frame block is the one place the source shape is load-bearing: the ROM
   keeps TWO hoisted zero slots ([sp+0x10] for the clamp, [sp+0x14] for the flag)
   and `1` in fp. `bool ok = false;` declared before the clamp merges both zeros
   into one register; assigning `ok = false;` AFTER `if (n >= 3) n = 0;` keeps them
   distinct, which is what the cartridge did. */
int daHanachan_c::Behavior()
{
    void *animFile;
    s32 i;
    Vector3 head;
    Vector3 part;
    Vector3 camA;
    Vector3 camB;
    Vector3 scratch;
    Vector3 t0;
    Vector3 t1;

    if (unk_8df != 0) {
        if (unk_8d0 + mPosY < -0x1f4000)
            MarkForDestruction();
    }

    func_ov034_02112604(this);

    if (mVertSpeed <= 0) {
        _Z14ApproachLinearRsss(&unk_444[0].y, unk_8d8, (s16)((mHorzSpeed / 0x1000) * 0x1e));
    }

    if (mWithMeshClsn.JustHitGround() != 0) {
        mVertAccel = -0x1000;
        LandingDust(true);
    }

    if (mModelAnims[0].file == *(BCA_File **)((char *)data_ov034_02113888[0] + 4) ||
        mModelAnims[0].file == *(BCA_File **)((char *)data_ov034_02113874[0] + 4) ||
        mModelAnims[0].file == *(BCA_File **)((char *)data_ov034_0211389c[0] + 4)) {
        if (mWithMeshClsn.IsOnGround() != 0) {
            if ((((u32)mModelAnims[0].currFrame << 4) >> 16) == 6) {
                if (unk_8db == 2)
                    func_0201267c(0x10a, &mCamSpacePosX);
                else
                    func_0201267c(0x11d, &mCamSpacePosX);
            }
            if (unk_8db == 2) {
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(mPosX, mPosY, mPosZ);
            }
        }
    }

    animFile = data_ov034_021138b0[0];
    if (mModelAnims[0].file == *(BCA_File **)((char *)animFile + 4) &&
        (((u32)mModelAnims[0].currFrame << 4) >> 16) == 0xf) {
        func_0201267c(0x11e, &mCamSpacePosX);
    }

    for (i = 0; i < 5; i++) {
        u16 n = 4 - unk_8db;
        bool ok;
        if (n >= 3) n = 0;
        ok = false;
        if (n <= 2) ok = true;
        if (ok)
            mTextureSequences[i].currFrame = n << 12;

        if (unk_8df == 0)
            mModelAnims[i].Advance();

        if (unk_8e0 == 0) {
            if (DecIfAbove0_Byte(&unk_702[i]) != 0) {
                unk_6f8[i] += 0x1200;
                unk_408[i].y = ((s32)(((s64)data_02082214[(((u16)unk_6f8[i] >> 4) << 1) + 1] * 0x3000 + 0x800) >> 12) + 0xc000) / 10;
            } else {
                _Z14ApproachLinearRiii(&unk_408[i].y, 0x1000, 0x199);
            }
        }
    }

    if (unk_8db == 4) {
        if (unk_8de != 0)
            mTextureSequences[0].currFrame = 0x1000;
        else
            mTextureSequences[0].currFrame = 0;
    }

    mPrevAngleY = unk_444[0].y;
    UpdatePos(0);

    if (unk_8df == 0)
        UpdateWMClsn(mWithMeshClsn, 0);

    unk_3cc[0].x = mPosX;
    unk_3cc[0].y = mPosY;
    unk_3cc[0].z = mPosZ;
    func_ov034_02112874(this);

    unk_3cc[0].y = unk_3cc[1].y + 0x1000;
    head = unk_3cc[0];
    head.x += (s32)(((s64)data_02082214[((u16)unk_444[0].y >> 4) << 1] * 0x64000 + 0x800) >> 12);
    head.z += (s32)(((s64)data_02082214[(((u16)unk_444[0].y >> 4) << 1) + 1] * 0x64000 + 0x800) >> 12);
    mdCc_cs1[0].pos = head;
    mdCc_cs2[0].pos = head;

    if (mModelAnims[0].file != *(BCA_File **)((char *)animFile + 4)) {
        u32 n = (u8)(4 - unk_8db);
        if (n < 3) {
            part = unk_3cc[0];
            part.x += (s32)(((s64)data_02082214[((u16)unk_444[0].y >> 4) << 1] * 0x23000 + 0x800) >> 12);
            part.z += (s32)(((s64)data_02082214[(((u16)unk_444[0].y >> 4) << 1) + 1] * 0x23000 + 0x800) >> 12);
            if (n == 0)
                part.y += 0x89000;
            else
                part.y += 0xb9000;
            if (unk_8de != 0) {
                unk_8e4 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    (u32)unk_8e4, data_ov034_02113828[n], part.x, part.y, part.z, 0, 0);
            }
        }
    }

    func_ov034_021129ec(this);

    {
        int j;
        for (j = 0; j < 5; j++) {
            mdCc_cs1[j].Clear();
            mdCc_cs1[j].Update();
            mdCc_cs2[j].Clear();
            mdCc_cs2[j].Update();
        }
    }

    if (unk_8e1 != 0) {
        void *cam = data_0209f318;
        camA.x = 0; camA.y = 0x80000; camA.z = 0;
        camB.x = 0; camB.y = 0x80000; camB.z = 0x1f4000;
        scratch.x = 0; scratch.y = 0; scratch.z = 0;
        Matrix4x3_FromRotationY(data_020a0e68, unk_444[0].y);
        MulVec3Mat4x3(&camA, data_020a0e68, &scratch);
        Vec3_Add(&t0, &mPosX, &scratch);
        camA = t0;
        scratch.x = 0; scratch.y = 0; scratch.z = 0;
        MulVec3Mat4x3(&camB, data_020a0e68, &scratch);
        Vec3_Add(&t1, &mPosX, &scratch);
        camB = t1;
        func_020092c4(cam, (char *)cam + 0x80, &camA);
        func_020092c4(cam, (char *)cam + 0x8c, &camB);
    }

    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- _ZN12daHanachan_c13InitResourcesEv, 0x0211323c, size 0x468 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daHanachan_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daHanachan_c::InitResources()
{
    char *c = (char *)((void *)this);
    s32 i;
    char *r8p;
    char *fpp;
    char *r7p;
    char *r6p;
    char *r5p;
    char *spC;
    char *sp10;
    char *sp14;
    void *sp18;
    void *sp1C;
    void *sp20;
    s32 sp24;
    s32 sp28;
    s32 sp2C;
    s32 sp30;
    s32 sp34;
    s32 sp38;
    s32 sp3C;
    s32 sp40;
    s32 sp44;
    s32 sp48;
    s32 va[3];
    s32 vb[3];
    s32 out[3];
    s32 j;
    char *r7_2;

    *(u8 *)(c + 0x8e2) = (u8) *(u32 *)(c + 8);
    if (*(u8 *)(c + 0x8e2) == 0xFF) {
        *(u8 *)(c + 0x8e2) = 0;
    }
    *(u8 *)(c + 0x8e3) = (u8) ((dActor_c *)c)->TrackStar(*(u8 *)(c + 0x8e2), 2);

    i = 0;
    sp10 = c + 0x478;
    sp14 = c + 0x5b8;
    sp40 = 0x78000;
    sp44 = 0x200000;
    sp48 = 0x32000;
    sp2C = 1;
    spC = c;
    r7p = c;
    r6p = c;
    r8p = c + 0x110;
    fpp = c + 0x368;
    r5p = c + 0x3cc;
    sp30 = -1;
    sp34 = 0;
    sp38 = 0;
    sp3C = 0;
    do {
        ((s32 *)(c + 0x464))[i] = data_ov034_021138c4[i];
        sp18 = data_ov034_02113838[i];
        Model::LoadFile(*(SharedFilePtr *)sp18);
        sp20 = data_ov034_0211384c[i];
        TextureSequence::LoadFile(*(SharedFilePtr *)sp20);
        sp1C = data_ov034_02113860[i];
        Animation::LoadFile(*(SharedFilePtr *)sp1C);
        Animation::LoadFile(*(SharedFilePtr *)data_ov034_02113874[i]);
        Animation::LoadFile(*(SharedFilePtr *)data_ov034_02113888[i]);
        Animation::LoadFile(*(SharedFilePtr *)data_ov034_0211389c[i]);
        Animation::LoadFile(*(SharedFilePtr *)data_ov034_021138b0[i]);
        ((ModelBase *)r8p)->SetFile(*(BMD_File **)((char *)sp18 + 4), sp2C, sp30);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(r8p, *(void **)((char *)sp1C + 4), sp34, 0x1000, sp34);
        TextureSequence::Prepare(**(BMD_File **)((char *)sp18 + 4), **(BTP_File **)((char *)sp20 + 4));
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(fpp, *(void **)((char *)sp20 + 4), sp38, 0x1000, sp38);
        *(s32 *)(spC + 0x374) = sp3C;
        if (i == 0) {
            *(s32 *)(r7p + 0x3cc) = *(s32 *)(c + 0x5c);
            *(s32 *)(r7p + 0x3d0) = *(s32 *)(c + 0x60);
            *(s32 *)(r7p + 0x3d4) = *(s32 *)(c + 0x64);
            *(s16 *)(r6p + 0x444) = *(s16 *)(c + 0x8c);
            *(s16 *)(r6p + 0x446) = *(s16 *)(c + 0x8e);
            *(s16 *)(r6p + 0x448) = *(s16 *)(c + 0x90);
            *(s16 *)(c + 0x94) = *(s16 *)(r6p + 0x446);
        } else {
            va[2] = 0 - ((s32 *)(c + 0x464))[i];
            va[0] = sp3C;
            va[1] = sp3C;
            vb[0] = sp3C;
            vb[1] = sp3C;
            vb[2] = sp3C;
            Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x446));
            MulVec3Mat4x3(va, data_020a0e68, vb);
            sp24 = i - 1;
            Vec3_Add(out, (c + 0x3cc) + sp24 * 0xc, vb);
            *(s32 *)(r7p + 0x3cc) = out[0];
            *(s32 *)(r7p + 0x3d0) = out[1];
            *(s32 *)(r7p + 0x3d4) = out[2];
            *(s16 *)(r6p + 0x446) = *(s16 *)(c + sp24 * 6 + 0x446);
        }
        *(s32 *)(r7p + 0x408) = 0x1000;
        *(s32 *)(r7p + 0x40c) = 0x1000;
        *(s32 *)(r7p + 0x410) = 0x1000;
        sp28 = data_ov034_021138d8[i];
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(sp10, c, r5p, sp28 + 0xA000, sp40, sp44, 0x26FE0);
        _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(sp14, c, r5p, sp28, sp48, 0x200004, 0x26FE0);
        spC += 0x14;
        r8p += 0x64;
        sp10 += 0x40;
        fpp += 0x14;
        r7p += 0xc;
        r6p += 6;
        r5p += 0xc;
        sp14 += 0x40;
        i += 1;
    } while (i < 5);

    *(s32 *)(c + 0x9c) = -0x1000;
    *(s32 *)(c + 0xa0) = -0x64000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x110, *(void **)((char *)data_ov034_02113874[0] + 4), 0, 0x1000, 0xc);
    r7_2 = c + 0x110;
    r7_2 += 0x64;
    j = 1;
    do {
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(r7_2, *(void **)((char *)data_ov034_02113874[j] + 4), 0, 0x1000, (u16) (s16) ((5 - j) * 3));
        j += 1;
        r7_2 += 0x64;
    } while (j < 5);

    func_ov034_021125b8(c, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(c + 0x708, c, 0x14000, 0x14000, 0, 0);
    *(u8 *)(c + 0x8db) = 4;
    func_ov034_021129ec(c);
    *(s32 *)(c + 0x8cc) = *(s32 *)(c + 0x5c);
    *(s32 *)(c + 0x8d0) = *(s32 *)(c + 0x60);
    *(s32 *)(c + 0x8d4) = *(s32 *)(c + 0x64);
    return 1;
}

