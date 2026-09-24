//cpp
/* Recovered translation unit -- ov095/daObjFlamethrower_c, the flame-jet actor.
 *
 * .text span 0x0213682c..0x02136ed4, 4 functions, ROM ordinals 0..3, exactly
 * the contiguous linker run build/tu_map.json places here. All 4 are assembled
 * below and config/tu_manifest.d/ov095/daObjFlamethrower_c.json names every
 * one, so the run has no hole and the 4 one-function files under src/ that
 * used to own these bytes are deleted by this change.
 *
 * IDENTITY IS THE CARTRIDGE'S. ov095 holds _ZTS19daObjFlamethrower_c at
 * 0x021376b4 and _ZTI19daObjFlamethrower_c at 0x021376a8; the 0x84-byte table
 * at 0x021376f0 is this class's own vtable, installed by
 * daObjFlamethrower_c_classInit at 0x02136ed4, which backs the
 * OBJ_FLAMETHROWER registry profile at 0x021376cc. No renaming was needed and
 * none was done.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. Deferred code
 * generation is left at its default (on), so mwccarm emits one .text section
 * per function in the REVERSE of source order: the highest-address ROM
 * function (_ZN19daObjFlamethrower_c13InitResourcesEv, 0x02136d60) is written
 * FIRST and the lowest written LAST. Do not reorder.
 *
 * THE DESTRUCTOR HAS NO DEFINITION HERE, AND THAT IS THE LOAD-BEARING PART.
 * The cartridge puts D1 first (0x0213682c, 0x58) and D0 second (0x02136884,
 * 0x6c). daObjFlamethrower_c.h gives the destructor an inline body, so under
 * deferred codegen mwccarm emits exactly that pair, in that order, ahead of
 * every written member -- which is the cartridge's order -- and emits no
 * homeless D2 at all. Writing the destructor out of line here instead was
 * measured on this TU and loses twice over: it emits D2, D0, D1 (D0 ahead of
 * the D1 the cartridge puts first), and `tubuild.py linkcheck` hard-refuses a
 * TU whose licensed .text is not ROM-ascending. `#pragma defer_codegen off`
 * plus an ascending rewrite was measured too: it fixes the member order but
 * leaves the destructor pair emitted LAST, after both members, which is worse.
 * Neither variant has a definition to hang a `// @symbol` marker on; both are
 * scored through the inline lifecycle definition in the class header.
 *
 * KEY-FUNCTION SIDE EFFECT: none claimed. With the destructor inline, the key
 * function is InitResources -- the first declared non-inline virtual -- and
 * this TU defines it, so the raw object also materializes _ZTV/_ZTI/_ZTS for
 * this class plus the dActor_c/dBase_c/fBase_c ancestor RTTI chain. The
 * promotion is text-only: the manifest's compiler_only_output externalizes all
 * nine records to their canonical public addresses (deadstrip-data) and claims
 * no data or BSS range. Licensing the ov095 side of that as real storage would
 * need two intervals, because g_profile_OBJ_FLAMETHROWER at 0x021376cc sits
 * between the RTTI records and the vtable; that is left for a later
 * data-owning promotion.
 *
 * NO `#pragma opt_strength_reduction off`. Both the Behavior and the
 * InitResources shards carried it; `tubuild create` flagged it [NOT carried]
 * and neither was re-added. The merged TU reaches 4/4 MATCH without them, so
 * they are not re-added on spec.
 *
 * WHAT IS STILL UNRECONSTRUCTED, deliberately, and recorded rather than
 * silently carried:
 *   1. Both members address the actor through `char *` offsets rather than the
 *      named fields daObjFlamethrower_c.h already declares. The named-member
 *      spelling has not been proved byte-identical, so the proven offset form
 *      stands and the readability work remains open.
 *   2. MulVec3Mat4x3 and Vec3_Add keep the `int *` spelling the Behavior shard
 *      used; InitResources casts at its three call sites. The two shards
 *      disagreed on the spelling and `struct Vec3` is layout-identical to the
 *      three-int prefix, so the casts are type-only and codegen-neutral.
 *   3. Sound::PlayLong, dCc_c::Clear/Update, dActor_c::FindWithID,
 *      Player::Burn and dCcPos_c::Init stay spelled as mangled externs: no
 *      shared header declares them in a form that keeps these call shapes.
 *
 * The 4 legacy one-function sources this folds are named by the manifest's
 * `legacy_source` fields (history); they are not repeated here.
 */

#include "daObjFlamethrower_c.h"
#include "decl_common.h"

/* Layout-identical three-int stand-in for the vector the two math helpers
 * take; see note 2 in the header comment above. */
struct Vec3 { int x, y, z; };

extern "C" {
extern signed char data_0209f2f8;
extern short data_02082214[];
extern short data_ov095_02136f98[];
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, const Vector3 *v, s16 e);
extern void MulVec3Mat4x3(int *v, void *m, int *dst);
extern void Vec3_Add(int *out, int *a, int *b);
extern char *_ZN8dActor_c10FindWithIDEj(u32 id);
extern void _ZN6Player4BurnEv(char *thiz);
extern void func_02022774(Fix12i x, Fix12i y, Fix12i z, s16 ang1, int ang2);
extern void func_020226fc(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2);
extern void func_02022864(Fix12i x, Fix12i y, Fix12i z, s16 ang1, int ang2);
extern void func_020227ec(Fix12i x, Fix12i y, Fix12i z, s16 ang1, s16 ang2);
extern void _ZN5dCc_c5ClearEv(void *thiz);
extern void _ZN5dCc_c6UpdateEv(void *thiz);
extern void _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(void *self, struct Vec3 *pos, int fix, int t, unsigned int a, unsigned int b);
}

// @symbol _ZN19daObjFlamethrower_c13InitResourcesEv
int daObjFlamethrower_c::InitResources()
{
    int count;
    int i;
    int im1;
    int zero;
    int flag;
    char *p6;
    char *p5;
    char *p4;
    struct Vec3 in;
    struct Vec3 dst;
    struct Vec3 sum;
    struct Vec3 sum2;

    count = 0xc;
    if (data_0209f2f8 == 0xc) count = 9;
    Matrix4x3_FromRotationXYZExt(((char *)this) + 0x434, mAngleX, mAngleY, mAngleZ);
    zero = 0;
    flag = 0;
    i = 0;
    if (i < count) {
        p6 = ((char *)this);
        p5 = ((char *)this) + 0x3a4;
        p4 = ((char *)this) + 0xd4;
        do {
        im1 = i - 1;
        in.x = zero;
        in.y = zero;
        in.z = zero;
        dst.x = zero;
        dst.y = zero;
        dst.z = zero;
        in.z = data_ov095_02136fb0[i];
        MulVec3Mat4x3((int *)&in, ((char *)this) + 0x434, (int *)&dst);
        if (im1 < 0) {
            Vec3_Add((int *)&sum, (int *)((char *)&mPosX), (int *)&dst);
            *(int *)(p6 + 0x3a4) = sum.x;
            *(int *)(p6 + 0x3a8) = sum.y;
            *(int *)(p6 + 0x3ac) = sum.z;
        } else {
            Vec3_Add((int *)&sum2, (int *)(((char *)this) + 0x3a4 + im1 * 0xc), (int *)&dst);
            *(int *)(p6 + 0x3a4) = sum2.x;
            *(int *)(p6 + 0x3a8) = sum2.y;
            *(int *)(p6 + 0x3ac) = sum2.z;
        }
        {
            short v = data_ov095_02136f80[i];
            _ZN8dCcPos_c4InitERK7Vector35Fix12IiES4_jj(p4, (struct Vec3 *)p5, v * 0x14, v * 0x28, 0x200002, flag);
        }
        p6 += 0xc;
        p5 += 0xc;
        p4 += 0x3c;
        i++;
        } while (i < count);
    }
    return 1;
}

// @symbol _ZN19daObjFlamethrower_c8BehaviorEv
int daObjFlamethrower_c::Behavior()
{
    char *self;
    int count;
    int i;
    int im1;
    int zero;
    int one;
    int no;
    int ang0;
    int ang1;
    char *ppos;
    int rot;
    char *pclsn;
    int j;
    int in[3];
    int dst[3];
    /* The y-bump below is interleaved into the argument build in the
     * cartridge; `volatile` pins that interleave without the shards'
     * `#pragma opt_strength_reduction off`. Not a live value. */
    volatile int part[3];
    int sum[3];
    int sum2[3];
    int y;
    int t;
    u32 id;
    char *pl;
    int b;
    s16 ang;
    int a2;
    u8 *p;

    self = (char *)this;
    count = 0xc;
    if (data_0209f2f8 == 0xc)
        count = 9;
    if (count <= 1)
        count = 1;
    if (count >= 0xc)
        count = 0xc;

    switch (*((u8 *)(self + 0x464))) {
    case 0:
        if (*((u8 *)(self + 0x465)) != 0) {
            p = (u8 *)((self + 0x400) + 0x65);
            *p = (u8)(*p - 1);
        }
        if (DecIfAbove0_Short((u16 *)(self + 0x466)) == 0) {
            *((u8 *)(self + 0x464)) = 1;
            *((u16 *)((self + 0x400) + 0x66)) = 0x3c;
        }
        break;
    case 1:
        if ((int)(*((u8 *)(self + 0x465))) < (count - 1)) {
            p = (u8 *)((self + 0x400) + 0x65);
            *p = (u8)(*p + 1);
        }
        if (DecIfAbove0_Short((u16 *)(self + 0x466)) == 0) {
            *((u8 *)(self + 0x464)) = 0;
            *((u16 *)((self + 0x400) + 0x66)) = 0x3c;
        }
        break;
    }

    if (*((u8 *)(self + 0x465)) != 0) {
        *((u32 *)(self + 0x468)) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
            *((u32 *)(self + 0x468)), 3, 0x180, (const Vector3 *)(self + 0x74), 0);
    }

    if (*((u32 *)(self + 0xb0)) & 8)
        t = 1;
    else
        t = 0;
    if (t != 0)
        return 1;

    if (((*((u32 *)(self + 8)) & 0xff) != 0xff) || (data_0209f2f8 == 0xc)) {
        Matrix4x3_FromRotationXYZExt(self + 0x434,
            *((s16 *)(self + 0x8c)), *((s16 *)(self + 0x8e)), *((s16 *)(self + 0x90)));
    }

    if (*((u8 *)(self + 0x465)) != 0) {
        zero = 0;
        one = 1;
        no = 0;
        ang0 = 0;
        ang1 = 0;
        i = 0;
        if (i < count) {
            ppos = self;
            rot = i;
            pclsn = self;
            do {
                if (i == (int)(*((u8 *)(self + 0x465))))
                    break;
                if (((*((u32 *)(self + 8)) & 0xff) != 0xff) || (data_0209f2f8 == 0xc)) {
                    im1 = i - 1;
                    in[0] = zero;
                    in[1] = zero;
                    in[2] = zero;
                    dst[0] = zero;
                    dst[1] = zero;
                    dst[2] = zero;
                    in[2] = data_ov095_02136fb0[i];
                    MulVec3Mat4x3(in, self + 0x434, dst);
                    if (im1 < 0) {
                        Vec3_Add(sum, (int *)(self + 0x5c), dst);
                        *((int *)(ppos + 0x3a4)) = sum[0];
                        *((int *)(ppos + 0x3a8)) = sum[1];
                        *((int *)(ppos + 0x3ac)) = sum[2];
                    } else {
                        Vec3_Add(sum2, (int *)(self + 0x3a4 + im1 * 0xc), dst);
                        *((int *)(ppos + 0x3a4)) = sum2[0];
                        *((int *)(ppos + 0x3a8)) = sum2[1];
                        *((int *)(ppos + 0x3ac)) = sum2[2];
                    }
                    if (data_0209f2f8 == 0xc) {
                        s16 ts = data_02082214[(((u16)(s16)rot) >> 4) * 2];
                        *((int *)(ppos + 0x3a8)) = (*((int *)(self + 0x60)))
                            + ((int)(((((long long)ts) * (-0xaa000)) + 0x800) >> 12));
                    }
                    *((int *)(pclsn + 0x104)) = *((int *)(ppos + 0x3a4));
                    *((int *)(pclsn + 0x108)) = *((int *)(ppos + 0x3a8));
                    *((int *)(pclsn + 0x10c)) = *((int *)(ppos + 0x3ac));
                }
                id = *((u32 *)(pclsn + 0xf8));
                if (id != 0) {
                    pl = _ZN8dActor_c10FindWithIDEj(id);
                    if (pl != 0) {
                        if (*((u16 *)(pl + 0xc)) == 0xbf)
                            b = one;
                        else
                            b = no;
                        if (b != 0)
                            _ZN6Player4BurnEv(pl);
                    }
                }
                part[0] = *((int *)(ppos + 0x3a4));
                y = *((int *)(ppos + 0x3a8));
                part[1] = y;
                y = y + 0x32000;
                part[2] = *((int *)(ppos + 0x3ac));
                part[1] = y;
                if ((*((u32 *)(self + 8)) & 0xff) != 0xff) {
                    ang = data_ov095_02136f80[i];
                    a2 = ang0;
                    func_02022774(part[0], part[1], part[2], ang, a2);
                    func_020226fc(part[0], part[1], part[2], ang, data_ov095_02136f98[i]);
                } else {
                    ang = data_ov095_02136f80[i];
                    a2 = ang1;
                    func_02022864(part[0], part[1], part[2], ang, a2);
                    func_020227ec(part[0], part[1], part[2], ang, data_ov095_02136f98[i]);
                }
                ppos += 0xc;
                rot += 0x1000;
                pclsn += 0x3c;
                i++;
            } while (i < count);
        }
    }

    j = 0;
    if (count > 0) {
        char *pc = self + 0xd4;
        do {
            _ZN5dCc_c5ClearEv(pc);
            if (j < (int)(*((u8 *)(self + 0x465))))
                _ZN5dCc_c6UpdateEv(pc);
            j++;
            pc += 0x3c;
        } while (j < count);
    }
    return 1;
}
