//cpp
/* Production translation unit for ov064/daWater_Hakidasi_c.
 * 11 function(s), .text 0x02119330..0x02119a18. The water jet
 * (registry profile WATER_HAKIDASI).
 *
 * NAME: _ZTS18daWater_Hakidasi_c is "18daWater_Hakidasi_c" at ov064 0x0211c2f8;
 * _ZTI at 0x0211c2ec reads [__si_class_type_info, that string,
 * _ZTI12dEnemyBase_c]. The vtable's address point is 0x0211c334; the word
 * before it is that _ZTI. The tree previously called the class JetStream
 * (coined).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02119330), D0
 * (0x02119368), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is
 * ROM-ascending. The factory daWater_Hakidasi_c_classInit (0x02119a18) stays
 * in its own source, src/d_a_water_hakidasi.c.
 *
 * `#pragma opt_strength_reduction off` is required inside func_ov064_021193b4
 * and is turned back on immediately after that function: the pragma is
 * file-global, and the later members matched without it.
 */

#include "daWater_Hakidasi_c.h"
#include "SharedFilePtr.h"

#pragma defer_codegen off

struct Vec3 { int x, y, z; };
struct Vec3_16 { s16 x, y, z; };

struct Foo {
    char padding[0x31c];
    int field31c;
    int arr[20];
    int field370;
    int field374;
};

typedef int Fix12i;

struct C;
typedef int (C::*PMF)();
struct C { char pad[0x300]; PMF *pp; };

namespace Model { void LoadFile(SharedFilePtr& f); }

extern "C" {
extern void Matrix4x3_FromRotationY(void *m, s16 angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
extern void MulVec3Mat4x3(struct Vec3 *in, void *m, struct Vec3 *out);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    int a, int b, int x, int y, int z, int f, int cb);
extern void func_02012790(int a);
extern int data_020a0e68;
extern int SublevelToLevel(int);
extern int data_ov064_0211c934;
extern int IsStarCollected(int r0, int r1);
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *, dActor_c *a, Fix12i b, int c, unsigned int d, unsigned int e);
unsigned short DecIfAbove0_Short(unsigned short *p);
}

extern SharedFilePtr data_ov002_0210da10;
extern SharedFilePtr data_ov002_0210d9a8;
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;

#define M(p) (p)

/* D0 is the deleting destructor: it destroys through this class and its
 * bases, which is why more than one vptr store appears, then frees through an
 * inline operator delete, which is why nothing here mentions a heap. */
// @symbol _ZN18daWater_Hakidasi_cD1Ev
// @symbol _ZN18daWater_Hakidasi_cD0Ev
daWater_Hakidasi_c::~daWater_Hakidasi_c()
{
}

// @symbol func_ov064_021193b4
extern "C" {
int func_ov064_021193b4(char *c)
{
    struct Vec3_16 rot;
    struct Vec3 base;
    struct Vec3 off;
    struct Vec3 spos;
    struct Vec3 npos1;
    struct Vec3 npos2;
    char *p;
    int dist;
    char *spawned;
    int i;
    char *hit;
    int uid;
    int *pp;
    int px, py, pz, nx, ny, nz;
    int *p370;
    int *p30c;
    int *p374;
    int *p310;
    int *posp;
    u16 ax, ay, az;
    s16 tx;
    int m1;

    p = (char *)((dActor_c *)c)->ClosestPlayer();
    if (p != 0 && *(u8 *)(p + 0x6f9) == 0) {
        dist = ((dActor_c *)c)->DistToCPlayer();
        if (dist < 0x3e8000) {
            dist = (0x3e8000 - dist) / 30;
            base.x = 0;
            base.y = 0;
            base.z = dist;
            off.x = 0;
            off.y = 0;
            off.z = 0;
            Matrix4x3_FromRotationY(&data_020a0e68, (s16)(((dActor_c *)c)->HorzAngleToCPlayer() + 0x8000));
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, -0x4000);
            MulVec3Mat4x3(&base, &data_020a0e68, &off);
            pp = (int *)(int)M(p + 0x5c);
            px = pp[0];
            base.x = px;
            py = pp[1];
            base.y = py;
            pz = pp[2];
            base.z = pz;
            {
                int tnx = px + off.x;
                base.y = py + off.y;
                base.z = pz + off.z;
                base.x = tnx;
                *(int *)(p + 0x5c) = tnx;
                *(int *)(p + 0x60) = base.y;
                *(int *)(p + 0x64) = base.z;
            }
        }
        ((dCc_c *)(c + 0x110))->Clear();
        ((dCc_c *)(c + 0x110))->dCc_c::Update();
        if (dist > 0x7d0000 && *(int *)(c + 0x30c) < 5) {
            *(int *)(c + 0x30c) = 0;
        }
    }

    if (*(int *)(c + 0x318) == 0) {
        *(int *)(c + 0x304) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x304), 0x138,
            *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64), 0, 0);
        if (*(int *)(c + 0x30c) < 5) {
            if (*(u16 *)(c + 0x100) == 0) {
                ax = *(volatile u16 *)(c + 0x8c);
                ay = *(volatile u16 *)(c + 0x8e);
                {
                    volatile struct Vec3_16 *vr = &rot;
                    vr->x = ax;
                    vr->y = ay;
                    az = *(u16 *)(c + 0x90);
                    tx = (s16)vr->x;
                    m1 = -1;
                    vr->z = az;
                    tx = (s16)(tx + 0x4000);
                    vr->x = (u16)tx;
                }
                spawned = (char *)dActor_c::Spawn(0xf4, 2, *(Vector3 *)(c + 0x5c), (Vector3_16 *)&rot, *(signed char *)(c + 0xcc), -1);
                if (spawned != 0) {
                    *(int *)(c + 0x320 + (*(int *)(c + 0x370) * 4)) = *(int *)(spawned + 4);
                    p370 = (int *)(int)M(c + 0x370);
                    *p370 = *p370 + 1;
                    if (*(int *)(c + 0x370) >= 0x14) {
                        *(int *)(c + 0x370) = 0;
                    }
                    *(char **)(spawned + 0x38c) = c;
                    *(int *)(spawned + 0xa4) = 0;
                    *(int *)(spawned + 0xa8) = 0x5000;
                    *(int *)(spawned + 0xac) = 0;
                }
                *(u16 *)(c + 0x100) = 0x50;
            }
            hit = *(char **)(c + 0x31c);
            if (hit != 0) {
                if (*(int *)(c + 0x30c) == 0) {
                    uid = *(int *)(hit + 4);
#pragma opt_strength_reduction off
                    for (i = 0; i < 0x14; i++) {
                        int slot = *(int *)(c + 0x320 + i * 4);
                        if (slot == uid) {
                            *(int *)(c + 0x374) = i;
                            p30c = (int *)(int)M(c + 0x30c);
                            *p30c = *p30c + 1;
                            func_02012790(0x25);
                            hit = *(char **)(c + 0x31c);
                            posp = (int *)(int)M(hit + 0x5c);
                            npos1.x = posp[0];
                            npos1.y = posp[1];
                            npos1.z = posp[2];
                            ((dActor_c *)c)->SpawnNumber(*(Vector3 *)&npos1, *(unsigned int *)(c + 0x30c), 0, 0, 0);
                            *(char **)(c + 0x31c) = 0;
                            return 1;
                        }
                    }
                } else {
                    p374 = (int *)(int)M(c + 0x374);
                    *p374 = *p374 + 1;
                    if (*(int *)(c + 0x374) >= 0x14) {
                        *(int *)(c + 0x374) = 0;
                    }
                    {
                        uid = *(int *)(*(char **)(c + 0x31c) + 4);
                        i = *(int *)(c + 0x374);
                        while (1) {
                            int slot = *(int *)(c + 0x320 + i * 4);
                            if (slot == uid) {
                                p30c = (int *)(int)M(c + 0x30c);
                                *p30c = *p30c + 1;
                                func_02012790(0x25);
                                hit = *(char **)(c + 0x31c);
                                posp = (int *)(int)M(hit + 0x5c);
                                npos2.x = posp[0];
                                npos2.y = posp[1];
                                npos2.z = posp[2];
                                ((dActor_c *)c)->SpawnNumber(*(Vector3 *)&npos2, *(unsigned int *)(c + 0x30c), 0, 0, 0);
                                *(char **)(c + 0x31c) = 0;
                                return 1;
                            }
                            break;
                        }
                    }
                }
                *(int *)(c + 0x30c) = 0;
                *(int *)(c + 0x374) = 0;
                *(char **)(c + 0x31c) = 0;
            }
        }
    } else {
        *(int *)(c + 0x304) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x304), 0x7b,
            *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64), 0, 0);
    }

    if (*(int *)(c + 0x30c) == 5) {
        p310 = (int *)(int)M(c + 0x310);
        spos.x = *(int *)(c + 0x5c);
        spos.y = *(int *)(c + 0x60);
        spos.z = *(int *)(c + 0x64);
        spos.y = spos.y - 0x64000;
        *p310 = *p310 + 1;
        if (*(int *)(c + 0x310) > 0x1e) {
            dActor_c::Spawn(0xb2, *(int *)(c + 0x314) | 0x40, *(Vector3 *)&spos, (Vector3_16 *)(c + 0x8c), *(signed char *)(c + 0xcc), -1);
            *(int *)(c + 0x30c) = 0xa;
        }
    }
    return 1;
}
}

#pragma opt_strength_reduction on

// @symbol func_ov064_021197fc
extern "C" int func_ov064_021197fc(Foo *c) {
    int i = 0;
    c->field31c = i;
    c->field370 = i;
    c->field374 = i;
    for (int v = i; i < 20; i++) {
        c->arr[i] = v;
    }
    return 1;
}

// @symbol func_ov064_0211982c
extern "C" int func_ov064_0211982c(C *c, PMF *p) { c->pp = p; PMF *q = c->pp; if (*q == 0) return 1; return (c->**q)(); }

// @symbol func_ov064_0211987c
extern "C" void func_ov064_0211987c(void *c)
{
}

/* Gives back the two shared files the jet renders from. Both live in ov002,
 * not in this overlay: the class borrows models the always-resident module
 * owns, so the handles are released rather than freed. */
// @symbol _ZN18daWater_Hakidasi_c16CleanupResourcesEv
s32 daWater_Hakidasi_c::CleanupResources()
{
    data_ov002_0210da10.Release();
    data_ov002_0210d9a8.Release();
    return 1;
}

/* Empty: the ROM body is a single `bx lr`. The override exists to suppress
 * whatever the base does on pending destroy. */
// @symbol _ZN18daWater_Hakidasi_c16OnPendingDestroyEv
void daWater_Hakidasi_c::OnPendingDestroy()
{
}

/* `mov r0,#1; bx lr` and nothing else: the class draws nothing of its own
 * from the render slot. */
// @symbol _ZN18daWater_Hakidasi_c6RenderEv
s32 daWater_Hakidasi_c::Render()
{
    return 1;
}

/* The state pointer at 0x300 is daWater_Hakidasi_c::State (see the header),
 * the same shape and treatment as Bullet::Behavior. */
// @symbol _ZN18daWater_Hakidasi_c8BehaviorEv
s32 daWater_Hakidasi_c::Behavior()
{
  DecIfAbove0_Short((unsigned short*)&mStateTimer);
  State* h = mState;
  /* Reads the handler's pointer word directly rather than as `&h->mMain`:
     taking the ADDRESS of a pointer-to-member makes mwcc materialise the whole
     8-byte pmf. Reading one to CALL it is free. */
  if (*(int*)((char*)h + 8) != 0) {
    (this->*(h->mMain))();
  }
  mAngleX = mPrevAngleX;
  mAngleY = mPrevAngleY;
  mAngleZ = mPrevAngleZ;
  func_ov064_0211987c(this);
  return 1;
}

// @symbol _ZN18daWater_Hakidasi_c13InitResourcesEv
/* Signature of _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj deliberately keeps
   Fix12<int> as a plain int: mwccarm passes that class by value differently
   when the parameter is spelled as the real type. See notes/mwccarm-codegen.md 6az. */
s32 daWater_Hakidasi_c::InitResources()
{
    Model::LoadFile(data_ov002_0210da10);
    Model::LoadFile(data_ov002_0210d9a8);
    unk_314 = (param1 >> 0xc) & 0xf;
    unk_318 = param1 & 1;
    if ((param1 & 0xf) > 1) unk_318 = 0;
    if (data_0209f2f8 == 8 && (data_0209f220 == 1 || IsStarCollected(SublevelToLevel(8), 1) == 0)) {
        return 0;
    }
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, (dActor_c*)this, 0xc8000, 0x190000, 0x800004, 0);
    func_ov064_0211982c((C *)this, (PMF *)&data_ov064_0211c934);
    return 1;
}
