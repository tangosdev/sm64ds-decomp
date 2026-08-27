//cpp
/* SHADOW translation unit -- ov002 / dBgActor_c (RTTI name dBgActor_c).
 *
 * NOT ENROLLED, AND NOT CANONICAL. This file contributes nothing to the ROM
 * build. The eleven one-function sources under src/ remain the enrolled owners
 * of 0x020ee42c..0x020eea84. See notes/translation-unit-reconstruction-plan.md
 * (the design) and notes/tu-reconstruction-pilot-report.md (the worked example).
 *
 * HAND-ASSEMBLED. `tubuild.py create` refuses this TU: three of the eleven
 * legacy sources wrap their body in `extern "C" { ... }`, which its splitter
 * cannot parse.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the REVERSE of their order in the source text, so the
 * HIGHEST-address ROM function is written FIRST. Reading order below is
 * therefore the constructor (0x020eea50), IsClsnInRangeOnScreen, IsClsnInRange,
 * UpdateModelPosAndRotY, UpdateClsnPosAndRot, UpdateKillByMegaChar,
 * func_ov002_020ee5d0, Kill, KillByMegaChar -- and the destructor variant group
 * last, which is where the ROM puts it (D1 0x020ee42c, D0 0x020ee464, the two
 * LOWEST addresses in the run).
 *
 * THE DESTRUCTORS ARE NOT WRITTEN OUT HERE ON PURPOSE. ~dBgActor_c is defined
 * inline in include/dBgActor_c.h -- load-bearing, seventy-odd subclasses inline
 * its body rather than calling it -- and Kill, defined below, is this class's
 * key function. Defining the key function is what makes this TU emit
 * _ZTV10dBgActor_c, _ZTI10dBgActor_c and the D1/D0/D2 variant group. The two legacy
 * files src/game/actors/dBgActor_c/_ZN10dBgActor_cD1Ev.cpp and src/game/actors/dBgActor_c/_ZN10dBgActor_cD0Ev.cpp exist only
 * because a per-function file has no key function to hang them on and needs a
 * forcing function (`p->~dBgActor_c()` / `delete p`); merged, that scaffolding is
 * exactly what we do NOT want, since it would add two .text sections the
 * cartridge does not have.
 *
 * WHAT STAYS SPELLED AS A MANGLED extern "C" NAME, AND WHY. Four members below
 * are defined under their mangled linker symbols with scalar parameters rather
 * than as real C++ methods:
 *
 *     _ZN10dBgActor_cC2Ev
 *     _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_
 *     _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_
 *     _ZN10dBgActor_c14KillByMegaCharER6Player
 *
 * The two IsClsnInRange* names claim `Fix12<int>` parameters BY VALUE. That
 * claim is a reconstruction, not a measurement -- the cartridge's RTTI carries
 * class names only, never parameter types -- and include/dActor_c.h section
 * "Methods whose mangled names carry a by-value class parameter" records the
 * measured consequence of believing it: CW homes a class-typed by-value
 * parameter to the stack, costing +0x14, so the real-method form misses. Note
 * that include/types.h's `Fix12i` is `typedef s32`, a scalar, and is NOT the
 * `Fix12<int>` class template in include/math/Fix12.h that these names encode;
 * the two are deliberately different spellings and only the scalar one
 * reproduces here.
 *
 * The constructor is held back for a different reason, recorded as a finding
 * rather than fixed: the ROM has an out-of-line _ZN10dBgActor_cC2Ev, which means
 * the original class DECLARED its constructor, but include/dBgActor_c.h does not.
 * Adding that declaration would stop every subclass constructor from inlining
 * the implicit one, and this TU cannot measure that blast radius on its own.
 *
 * KillByMegaChar's name claims `Player&` and the header agrees, but its legacy
 * body reaches five unnamed dActor_c fields (0x098, 0x09c, 0x0a0, 0x0a8) by raw
 * offset and calls dActor_c::Earthquake, which takes a by-value Fix12<int> and so
 * hits the same homing rule. Left in legacy form.
 */
#include "dBgActor_c.h"
#include "dBgW.h"

/* ------------------------------------------------------------------------- */
/* ROM ordinal 10 -- _ZN10dBgActor_cC2Ev, 0x020eea50, size 0x34                  */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header, vtable identified, globals resolved
 * resolved: VT0 = _ZTV10dBgActor_c
 *
 * THE `+ 2` IS LOAD-BEARING, and consolidation is what forced it. The legacy
 * file declared `extern int _ZTV10dBgActor_c[]` by the ROM's convention, where the
 * symbol IS the slot array, and stored it directly. This TU now DEFINES the
 * vtable, because it defines the class's key function, and mwcc's own
 * convention puts the symbol at the start of the vtable OBJECT -- two words of
 * Itanium preamble (offset-to-top, typeinfo pointer) earlier. Adding 2 to the
 * int-array spelling lands on the first slot either way.
 */
extern "C" {
extern int   _ZTV10dBgActor_c[];
extern int  *_ZN8dActor_cC2Ev(int *thiz);
extern void  _ZN5ModelC1Ev(void *thiz);
extern void  _ZN10dBgW_KcMbgC1Ev(void *thiz);

int *_ZN10dBgActor_cC2Ev(int *t)
{
    _ZN8dActor_cC2Ev(t);
    t[0] = (int)(_ZTV10dBgActor_c + 2);
    _ZN5ModelC1Ev((char *)t + 0xd4);
    _ZN10dBgW_KcMbgC1Ev((char *)t + 0x124);
    return t;
}
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 9 -- IsClsnInRangeOnScreen, 0x020ee934, size 0x11c             */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header */
extern "C" {
extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern int   Vec3_Dist(void *a, void *b);

int _ZN10dBgActor_c21IsClsnInRangeOnScreenE5Fix12IiES1_(struct dBgActor_c *self, int a, int b)
{
    int on = (self->mFlags & 8) != 0;
    if (on) {
        if (((dBgW *)((char *)&self->mMeshCollider))->IsEnabled())
            ((dBgW *)((char *)&self->mMeshCollider))->Disable();
        return 0;
    }
    if (a == 0) {
        if (!((dBgW *)((char *)&self->mMeshCollider))->IsEnabled())
            ((dBgW *)(((char *)self) + 0x124))->Enable((dActor_c *)(((char *)self)));
        goto done;
    }
    {
    struct Vector3 v;
    v.x = self->mPosX;
    v.y = self->mPosY;
    v.z = self->mPosZ;
    if (b == 0) v.y = v.y + self->mClipOffsetY;
    else v.y = v.y + b;
    void *p = _ZN8dActor_c13ClosestPlayerEv(((char *)self));
    int d = Vec3_Dist(&v, (char *)p + 0x5c);
    if (d > a) {
        if (((dBgW *)((char *)&self->mMeshCollider))->IsEnabled())
            ((dBgW *)((char *)&self->mMeshCollider))->Disable();
        return 0;
    }
    if (!((dBgW *)((char *)&self->mMeshCollider))->IsEnabled())
        ((dBgW *)(((char *)self) + 0x124))->Enable((dActor_c *)(((char *)self)));
    }
done:
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 8 -- IsClsnInRange, 0x020ee870, size 0xc4                      */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header. Declarations for ClosestPlayer and
   Vec3_Dist are the ones above, at their first occurrence -- not repeated. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(struct dBgActor_c *self, int a, int b)
{
    struct Vector3 v;
    v.x = self->mPosX;
    v.y = self->mPosY;
    v.z = self->mPosZ;
    if (a == 0) a = self->mClipRadius << 3;
    if (b == 0) v.y = v.y + self->mClipOffsetY;
    else v.y = v.y + b;
    void *p = _ZN8dActor_c13ClosestPlayerEv(((char *)self));
    int d = Vec3_Dist(&v, (char *)p + 0x5c);
    if (d > a) {
        if (((dBgW *)((char *)&self->mMeshCollider))->IsEnabled())
            ((dBgW *)((char *)&self->mMeshCollider))->Disable();
        return 0;
    }
    if (!((dBgW *)((char *)&self->mMeshCollider))->IsEnabled())
        ((dBgW *)(((char *)self) + 0x124))->Enable((dActor_c *)(((char *)self)));
    return 1;
}
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 7 -- dBgActor_c::UpdateModelPosAndRotY, 0x020ee830, size 0x40    */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method
 *
 * `((char *)this) + 0xf0` and the three unk_114/118/11c writes the generated
 * header used to carry were the same object reached two ways: 0xf0 is
 * mModel.mat4x3, and 0x114/0x118/0x11c are its translation row, m[9]..m[11].
 * Build the rotation into the model's matrix, then drop the position into it --
 * at 1/8 scale, which is the model-space unit.
 */
extern "C" void Matrix4x3_FromRotationY(Matrix4x3 *mat, int angleY);

void dBgActor_c::UpdateModelPosAndRotY()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.m[9]  = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 6 -- dBgActor_c::UpdateClsnPosAndRot, 0x020ee7cc, size 0x64      */
/* ------------------------------------------------------------------------- */
/* recovered: named members + shared header, real C++ method
 *
 * Take the model's transform, replace its translation row with the actor's
 * position, hand it to the collider. m[9], m[10] and m[11] are that row.
 *
 * The flat `s32 m[12]` spelling of Matrix4x3 is required here and is why
 * include/dBgActor_c.h includes common.h before Model.h: the `{Matrix3x3 r;
 * Vector3 t;}` spelling copies the two members separately, 9 words then 3, and
 * this function comes out 0x74 against the ROM's 0x64.
 */
extern "C" int _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(
    dBgW_KcMbg *self, const Matrix4x3 &mat, short angleY);

void dBgActor_c::UpdateClsnPosAndRot()
{
    mClsnMat = mModel.mat4x3;
    mClsnMat.m[9]  = mPosX;
    mClsnMat.m[10] = mPosY;
    mClsnMat.m[11] = mPosZ;
    _ZN10dBgW_KcMbg9TransformERK9Matrix4x3s(&mMeshCollider, mClsnMat, mAngleY);
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 5 -- dBgActor_c::UpdateKillByMegaChar, 0x020ee674, size 0x158    */
/* ------------------------------------------------------------------------- */
/* recovered: real C++ method.
 *
 * The legacy file reached every field through its own flat shadow structs -- a
 * `dBgActor_c` that was one big char array, its own `dActor_c`, `Vector3`,
 * `Matrix4x3` and a 32-slot `PlatformVT` whose only used entry was v31. All of
 * those are now the real types: v31 is dBgActor_c::Kill (the slot this class
 * adds), and the raw 0x8c/0x8e/0x90/0x94 stores are mAngleX/mAngleY/mAngleZ/
 * mPrevAngleY.
 *
 * `Fix12<int> d` BY VALUE IS THE ROM'S OWN SHAPE HERE, and it is worth being
 * precise about why that does not contradict the note at the top of this file.
 * The class template in include/math/Fix12.h is `{ T val; }` -- byte-identical
 * to the shadow `{ T v; }` the legacy file used and matched with. This one is
 * measured. The IsClsnInRange* names' Fix12<int> claims are not.
 *
 * dBgCh_Lin IS DECLARED LOCALLY AND AT 0x78, WHICH CONTRADICTS
 * include/dBgCh_Lin.h. That header's last field is unk_064 at 0x064, so it
 * describes a 0x68-byte object; the legacy body that reproduces these bytes
 * declares 0x78, and `ray` is a stack local whose size sets the frame. 0x78 is
 * what matches. include/dBgCh_Lin.h is not included here (it is not in
 * dBgActor_c.h's include closure, so there is no clash), and it is NOT edited --
 * that header is shared and re-sizing it is a separate, measured change. Filed
 * as a finding.
 */
extern "C" {
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void func_ov002_020ee5d0(unsigned char *self, int arg);
extern Matrix4x3 data_020a0e68;
}

/* Local, deliberately -- see the note above. Non-virtual, so sizeof is 0x78. */
struct dBgCh_Lin {
    dBgCh_Lin();
    ~dBgCh_Lin();
    void SetObjAndLine(Vector3 const &a, Vector3 const &b, dActor_c *c);
    int  DetectClsn();
    char buf[0x78];
};

int dBgActor_c::UpdateKillByMegaChar(short a, short b, short c, Fix12<int> d)
{
    Vector3 vin;
    Vector3 vmid;
    Vector3 vout;

    if (unk_31c == 0) return 0;

    vin.x = 0;
    vin.y = 0;
    vin.z = 0xc8000;
    vmid.x = 0;
    vmid.y = 0;
    vmid.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
    MulVec3Mat4x3(&vin, &data_020a0e68, &vmid);
    Vec3_Add(&vout, (Vector3 *)&mPosX, &vmid);
    vmid = vout;

    dBgCh_Lin ray;
    ray.SetObjAndLine(*(Vector3 *)&mPosX, vmid, this);
    /* THESE FOUR STAY RAW CASTS, and the byte gate is the only thing that could
       have told us so. Written as the named members they are --
       `mPrevAngleY = (short)(mPrevAngleY + 0x8000)`, `mAngleX = ...` -- mwcc
       folds each access into a direct `[r7,#0x94]` offset and the function comes
       out 0x148 against the ROM's 0x158. The cartridge instead materialises each
       field's address into a scratch register first (`add r1,r7,#0x94`,
       `add ip,r7,#0x8e`, `add r3,r7,#0x90`) and reuses it across the load and
       the store, which is what the `(char *)this + off` spelling produces. Four
       instructions, and they are the whole 0x10 difference.
       The choice is per-function, not per-tree: the same class's
       UpdateClsnPosAndRot and UpdateModelPosAndRotY match with named members.
       See notes/mwccarm-codegen.md on compound assignment CSEing field
       addresses. */
    if (ray.DetectClsn()) {
        *(short *)((int)((char *)this + 0x94)) =
            (short)(*(short *)((int)((char *)this + 0x94)) + 0x8000);
    }
    *(short *)((int)((char *)this + 0x8c)) =
        (short)(*(short *)((int)((char *)this + 0x8c)) + a);
    *(short *)((int)((char *)this + 0x8e)) =
        (short)(*(short *)((int)((char *)this + 0x8e)) + b);
    *(short *)((int)((char *)this + 0x90)) =
        (short)(*(short *)((int)((char *)this + 0x90)) + c);
    UpdatePos(0);
    if (DecIfAbove0_Byte(&unk_31d) == 0) {
        Kill();
    }
    func_ov002_020ee5d0((unsigned char *)this, d.val);
    return 1;
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov002_020ee5d0, 0x020ee5d0, size 0xa4               */
/* ------------------------------------------------------------------------- */
/* recovered: shared common types. A file-static helper of this TU: it takes the
   platform as a raw byte pointer and is called only from UpdateKillByMegaChar
   above. Kept under its plain ROM name, which needs no mangling either way. */
extern "C" {
extern void Vec3_Asr(struct Vector3 *d, struct Vector3 *s, int sh);
extern void Matrix4x3_FromTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZXYExt(void *m, int x, int y, int z);

void func_ov002_020ee5d0(unsigned char *self, int arg)
{
    struct Vector3 v;
    Vec3_Asr(&v, (struct Vector3 *)(self + 0x5c), 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, arg >> 3, 0);
    Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
        *(s16 *)(self + 0x8c), *(s16 *)(self + 0x8e), *(s16 *)(self + 0x90));
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, -arg >> 3, 0);
    *(struct Matrix4x3 *)(self + 0xf0) = data_020a0e68;
}
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 3 -- dBgActor_c::Kill, 0x020ee55c, size 0x74                     */
/* ------------------------------------------------------------------------- */
/* THE SLOT THIS CLASS ADDS, and this TU's key function. dActor_c's table ends at
 * slot 30; Kill is the one new virtual dBgActor_c declares, and 97 of the 101
 * classes deriving from it have exactly 32 slots because of this function.
 * Being the key function is also why THIS file emits _ZTV10dBgActor_c,
 * _ZTI10dBgActor_c and the destructor variants -- see the header comment.
 *
 * The particle spawns 0x64000 -- one 20.12 unit is 0x1000, so 100 units --
 * above the platform, and the poof and the sound both take the position by
 * reference. mCamSpacePos is read as a Vector3 through its first member, which
 * is how the rest of the tree spells that triple.
 *
 * Particle::System::NewSimple stays spelled as its mangled name: its parameters
 * are Fix12<int> BY VALUE, and declaring the true types changes how the caller
 * passes them and breaks the bytes -- notes/mwccarm-codegen.md 6az.
 */
#include "Sound.h"

extern "C" void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);

void dBgActor_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    Fix12i x = mPosX;
    Fix12i y = mPosY + 0x64000;
    Fix12i z = mPosZ;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xa, pos.x, pos.y, pos.z);
    /* MEMBERWISE, NOT `dustPos = pos`. Vector3 declares a destructor (types.h),
       so it is non-POD, and a whole-object assignment compiles to an ldm/stm
       pair -- four instructions where the ROM has six. Three field stores are
       what the cartridge does. */
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}

/* ------------------------------------------------------------------------- */
/* ROM ordinal 2 -- KillByMegaChar, 0x020ee4b0, size 0xac                     */
/* ------------------------------------------------------------------------- */
/* Legacy form -- see the header comment for why this one is not a real method
   yet. dActor_c::Earthquake takes a by-value Fix12<int>, so the true-signature
   declaration is the thing that would cost bytes, not the offsets. */
extern "C" {
extern void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *thiz, struct Vector3 *v, int f);
extern short Vec3_HorzAngle(struct Vector3 *a, struct Vector3 *b);
extern int _ZN4dBgW9IsEnabledEv(void *c);
extern void _ZN4dBgW7DisableEv(void *c);

void _ZN10dBgActor_c14KillByMegaCharER6Player(char *c, char *player)
{
    struct Vector3 v;
    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(c, &v, 0x5dc000);
    *(unsigned char *)(c + 0x31c) = 1;
    *(int *)(c + 0x9c) = -0x2000;
    *(int *)(c + 0xa0) = -0x3c000;
    *(int *)(c + 0x98) = 0x14000;
    *(int *)(c + 0xa8) = 0x1e000;
    *(short *)(c + 0x94) = Vec3_HorzAngle((struct Vector3 *)(player + 0x5c),
                                          (struct Vector3 *)(c + 0x5c));
    *(unsigned char *)(c + 0x31d) = 0x1e;
    if (_ZN4dBgW9IsEnabledEv(c + 0x124) == 0) return;
    _ZN4dBgW7DisableEv(c + 0x124);
}
}

/* ROM ordinals 1 and 0 -- _ZN10dBgActor_cD0Ev (0x020ee464) and _ZN10dBgActor_cD1Ev
   (0x020ee42c) -- are emitted implicitly by the key function above. Nothing is
   written for them here on purpose; see the header comment. */
