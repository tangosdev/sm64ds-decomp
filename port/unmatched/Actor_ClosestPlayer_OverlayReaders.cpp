/* HOST COPIES of four overlay bodies that each call Actor::ClosestPlayer() with
 * NO argument and rely on `this` riding ARM r0:
 *
 *     src/func_ov084_02129cf4.c    (ov084 chase/aim state; receiver `c`)
 *     src/func_ov084_0212f204.c    (ov084 range cache;       receiver `r4`)
 *     src/func_ov094_02136024.c    (ov094 approach state;    receiver `c`)
 *     src/func_ov102_02149078.c    (ov102 refusal test;      receiver `self`)
 *
 * THE r0-PASSTHROUGH SEAM -- identical to Actor_ClosestPlayerWrappers.cpp: each
 * body's own `this` is its first parameter (the ROM's r0), and it calls
 * Actor::ClosestPlayer() with no argument. Byte-identical on ARM because
 * ClosestPlayer reads `this` from r0 and the caller's r0 is still live across the
 * `bl`. On the host the ClosestPlayer definition is `(void *self)` cdecl, so a
 * zero-argument call passes stack garbage and ClosestPlayer's
 * Vec3_Dist((char*)self + 0x5c, ...) reads a garbage base + 0x5c -- the null-this
 * +0x5c fault, the same class as the live rabbit crash.
 *
 * THE FIX passes each body's own first parameter to ClosestPlayer -- exactly the
 * value the ROM leaves in r0. The bodies are the matched sources line for line;
 * only the ClosestPlayer declaration (now one-arg) and its call site change.
 *
 * The four byte-locked sources are commented out of their slice gates (32, 70,
 * 194, 203) in favour of these copies; the sources are unchanged.
 */

/* STILL-RAW Actor::ClosestPlayer zero-argument readers (LATENT, not yet hosted).
 *
 * Eight more readers open the same r0 passthrough seam but are NOT in the build
 * today: their overlays are not currently mounted in the port (map hits 0), so
 * host copying them now would only add dead code. Each is a latent live bug the
 * instant its overlay gets hosted. A dropped receiver makes ClosestPlayer read
 * off a null base (Vec3_Dist from garbage + 0x5c), the null this + 0x5c fault,
 * the same class as the live rabbit crash (see Actor_ClosestPlayerWrappers.cpp,
 * corpus id 9e164e92).
 *
 * NONE of the eight is listed in any port/slice_gate*.txt manifest (checked on
 * cons commit 24c2dcf75), so there is no arrival line to annotate; this block is
 * the registry instead. BEFORE un-commenting or otherwise hosting the overlay
 * that carries one of these, host copy the reader to PASS the receiver first
 * (exactly the value the ROM leaves in r0), the way the four copies below and
 * the wrappers file do. Per seat gate item 4: a newly hosted overlay carrying
 * known raw readers.
 *
 *   src/func_ov018_02111b3c.c                 (ov018, calls ClosestPlayer() no arg)
 *   src/func_ov020_02111fc4.cpp               (ov020, calls ClosestPlayer() no arg)
 *   src/func_ov032_02111350.c                 (ov032, calls ClosestPlayer() no arg)
 *   src/func_ov060_02111f08.c                 (ov060, calls ClosestPlayer() no arg)
 *   src/func_ov066_02119398.cpp               (ov066, calls ClosestPlayer() no arg)
 *   src/unnamed/ov063/func_ov063_02117650.c   (ov063, calls ClosestPlayer() no arg)
 *   src/_ZN5Actor23HorzAngleToCPlayerOrAngEv.c (Actor::HorzAngleToCPlayerOrAng, no arg;
 *                                              already noted in-slice as "in no slice")
 *   src/actors/Boo/_ZN3Boo13InitResourcesEv.c (Boo::InitResources: its two call sites
 *                                              already pass c, so it is safe on the host
 *                                              as written, but its extern is declared
 *                                              zero-arg style; keep the c argument if the
 *                                              declaration is ever reprototyped.)
 */
#include "common.h"

/* one real one-arg (this) shape, shared by all four copies below */
extern "C" void *_ZN5Actor13ClosestPlayerEv(void *self);

/* ---- func_ov084_02129cf4 (receiver c) ------------------------------------- */
extern "C" {
typedef int Fix12i;
int _ZNK12WithMeshClsn8IsOnWallEv(void *c);
Fix12i Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
}

// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes c).
extern "C" void func_ov084_02129cf4(char *c, Fix12i distThresh)
{
    struct Vector3 ppos;

    *(void **)(c + 0x438) = _ZN5Actor13ClosestPlayerEv(c);   /* <-- this, the ROM's r0 */

    if (*(void **)(c + 0x438) == 0
        || (Vec3_Dist((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c)) > distThresh
            && *(unsigned char*)(c+0x113) >= 6)) {
        *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c));
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }

    {
        int *ppos_src = (int *)(int)((long long)(int)(*(char **)(c + 0x438) + 0x5c));
        ppos.x = ppos_src[0];
        ppos.y = ppos_src[1];
        ppos.z = ppos_src[2];
    }

    if (*(unsigned char*)(c+0x113) < 6) {
        if (Vec3_Dist((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c)) > distThresh
            && !_ZNK12WithMeshClsn8IsOnWallEv(c+0x1b4)) {
            *(int*)(c+0x440) = 0x61a8000;
            *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), (struct Vector3*)(c+0x41c));
            return;
        }

        if (Vec3_Dist((struct Vector3*)(c+0x5c), &ppos) < *(int*)(c+0x448)) {
            *(int*)(c+0x440) = Vec3_Dist((struct Vector3*)(c+0x5c), &ppos);
            if (*(unsigned short*)(c+0x400+0x58) != 0) {
                *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), &ppos);
                return;
            }
            *(short*)(c+0x400+0x5a) = Vec3_HorzAngle(&ppos, (struct Vector3*)(c+0x5c));
            return;
        }
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }

    if (Vec3_Dist((struct Vector3*)(c+0x41c), &ppos) > distThresh) {
        *(int*)(c+0x440) = 0x61a8000;
        return;
    }
    *(int*)(c+0x440) = Vec3_Dist((struct Vector3*)(c+0x5c), &ppos);
    *(short*)(c+0x400+0x5a) = Vec3_HorzAngle((struct Vector3*)(c+0x5c), &ppos);
}

/* ---- func_ov084_0212f204 (receiver r4) ------------------------------------ */
// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes r4).
extern "C" void func_ov084_0212f204(char* r4)
{
    struct Vector3 v;
    *(char**)(r4 + 0x460) = (char*)_ZN5Actor13ClosestPlayerEv(r4);   /* <-- this, the ROM's r0 */
    {
        char* p = *(char**)(r4 + 0x460);
        if (p != 0) {
            struct Vector3* pp = (struct Vector3*)(((int)p + 0x5c));
            v.x = pp->x;
            v.y = pp->y;
            v.z = pp->z;
            *(int*)(r4 + 0x464) = Vec3_Dist((struct Vector3*)(r4 + 0x5c), &v);
            *(short*)(r4 + 0x468) = Vec3_HorzAngle((struct Vector3*)(r4 + 0x5c), &v);
            *(int*)(r4 + 0x46c) = *(unsigned char*)(*(char**)(r4 + 0x460) + 0x6de);
        } else {
            *(int*)(r4 + 0x464) = 0x7fffffff;
            *(short*)(r4 + 0x468) = *(short*)(r4 + 0x8e);
        }
    }
}

/* ---- func_ov094_02136024 (receiver c) ------------------------------------- */
extern "C" {
typedef struct { int x, y, z; } Ov094Vec3;
typedef struct Ov094PMF Ov094PMF;
void Vec3_Sub(Ov094Vec3* out, Ov094Vec3* a, Ov094Vec3* b);
int LenVec3(Ov094Vec3* v);
int func_ov094_02136188(void* c, Ov094PMF* p);
extern char data_020a0e68[];
extern Ov094PMF data_ov094_02136b60;
void Matrix4x3_FromRotationY(void* m, int angle);
void MulVec3Mat4x3(Ov094Vec3* in, void* m, Ov094Vec3* out);
}
#define OV094_LA(p) ((char*)(unsigned)(((long long)(unsigned)(unsigned)(p))))

// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes c).
extern "C" int func_ov094_02136024(char* c)
{
    char* p = (char*)_ZN5Actor13ClosestPlayerEv(c);   /* <-- this, the ROM's r0 */
    if (p != 0 && *(int*)(p+0x37c) != 0) {
        char* ip = OV094_LA(p + 0x5c);
        Ov094Vec3 pp;
        Ov094Vec3 d;
        Ov094Vec3* selfpos = (Ov094Vec3*)(c + 0x5c);
        pp.x = *(int*)(ip+0);
        pp.y = *(int*)(ip+4);
        pp.z = *(int*)(ip+8);
        *(int*)(c+0x60) = *(int*)(p+0x644) + 0x190000;
        *(int*)(c+0x3d0) = (int)p;
        Vec3_Sub(&d, selfpos, &pp);
        if (LenVec3(&d) < 0x28000) {
            d.x = *(int*)(c+0x5c);
            d.y = *(int*)(c+0x60);
            d.z = *(int*)(c+0x64);
            d.x = 0;
            d.y = 0;
            d.z = 0x320000;
            Matrix4x3_FromRotationY(data_020a0e68, 0x4000);
            MulVec3Mat4x3(&d, data_020a0e68, (Ov094Vec3*)(c+0x3d8));
            {
                int *px = (int*)OV094_LA(c + 0x3d8);
                int *py = (int*)OV094_LA(c + 0x3dc);
                int *pz = (int*)OV094_LA(c + 0x3e0);
                *px = *px + *(int*)(c+0x5c);
                *py = *py + *(int*)(c+0x60);
                *pz = *pz + *(int*)(c+0x64);
            }
            *(short*)(c+0x94) = Vec3_HorzAngle((const struct Vector3*)(c+0x5c), (const struct Vector3*)(c+0x3d8));
            *(short*)(c+0x8e) = *(short*)(c+0x94);
            func_ov094_02136188(c, &data_ov094_02136b60);
        }
    }
    return 1;
}

/* ---- func_ov102_02149078 (receiver self) ---------------------------------- */
extern "C" {
extern signed char data_0209f2f8;
extern int data_0209f32c;
}

// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes self).
extern "C" int func_ov102_02149078(void *self)
{
    if (data_0209f2f8 == 0x15) {
        if ((int)(*(int*)((char*)self + 0x3e0) - 0x32000) <= data_0209f32c)
            return 1;
    } else {
        if (data_0209f2f8 == 0x21)
            return 0;
        if (*(unsigned char*)((char*)_ZN5Actor13ClosestPlayerEv(self) + 0x706))   /* <-- this */
            return 1;
    }
    return 0;
}
