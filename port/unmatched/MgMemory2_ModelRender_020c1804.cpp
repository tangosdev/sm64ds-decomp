/* HOST COPY of src/func_ov006_020c1804.cpp -- the per-frame model draw of the
 * ov006 model sub-object dScMgMemory2_c holds at this+0x4f38, reached from
 * vtable slot 9 (Render) through src/func_ov006_020f73f4.c.
 * Run mg6, lane MEM.
 *
 * TWO INDEPENDENT DEFECTS IN ONE TU, and this lane hit both on its first boot.
 * Neither is visible to the byte gate and neither is visible to a link.
 *
 * ---- 1. THE LOCAL SHADOW CLASS, TWICE ------------------------------------
 *
 * The same defect port/unmatched/MgFlower_ModelRender_020c3bf4.cpp (run mg5,
 * lane Y3D) and port/unmatched/Door_Render.cpp record, at two call sites in
 * one function. hal/cxxname_bridge.cpp fills the host model vtables in MSVC
 * order, where the destructor costs ONE slot and Itanium's costs two:
 *
 *   _ZTV5Model           dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3,
 *                        Render 4
 *   _ZTV14BlendModelAnim dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3,
 *                        Render 4, Virtual18 5
 *
 * src spells both objects with a local `struct Obj` of SIX pure virtuals and
 * calls `f5(0)`, which is the ROM's own numbering: BYTE +0x14 of an Itanium
 * vtable is the sixth word, and on both model classes that word is Render.
 * Read straight off extracted/overlays/overlay_0006.bin at base 0x020bfec0:
 *
 *   020c1978  add  r0,r4,#0x1c
 *   020c197c  ldr  r2,[r0]            ; vptr of the BlendModelAnim at +0x1c
 *   020c1980  mov  r1,#0              ; scale = NULL
 *   020c1984  ldr  r2,[r2,#0x14]      ; ROM slot 5 -> Render
 *   020c1988  blx  r2
 *   020c198c  add  r0,r4,#0xdc / bl func_ov006_020c07a0
 *   020c1994  add  r0,r4,#0x8c
 *   020c1998  mov  r1,#0
 *   020c199c  ldr  r2,[r0]            ; vptr of the Model at +0x8c
 *   020c19a0  ldr  r2,[r2,#0x14]      ; ROM slot 5 -> Render
 *   020c19a4  blx  r2
 *   020c19a8  bl   ShadowModel::RenderAll
 *
 * On the host that byte is the SIXTH MSVC word, which on BlendModelAnim is
 * Virtual18(u32 mat, const Vector3 *scale). The shadow class passes one
 * argument, so mat arrives 0, and Virtual18's first statement is
 * Virtual10(*(Matrix4x3 *)mat) -- a matrix reference at address zero.
 *
 * MEASURED, scene 363, this tree, before this file existed:
 *
 *   FAULT code c0000005 at +0x00092cfc accessing 00000000
 *     mem_render (slot 9) -> func_ov006_020f73f4+0x42
 *       -> func_ov006_020c1804+0x186
 *       -> BlendModelAnim::Virtual18 -> BlendModelAnim::Virtual10
 *       -> Model::Virtual10+0xc,  eax=00000000
 *
 * which is the Y3D chain one class over, ending in the same
 * `movups xmm0,[eax]`.
 *
 * WHICH CLASS EACH OBJECT IS, established from the CONSTRUCTOR rather than
 * guessed from the call. src/func_ov006_020c1d80.c builds this sub-object and
 * runs `_ZN14BlendModelAnimC1Ev(t + 0x1c)` and `_ZN5ModelC1Ev(t + 0x8c)`, so
 * +0x1c is a BlendModelAnim and +0x8c is a Model. Both have Render at MSVC
 * slot 4 under the headers above.
 *
 * THE DISPATCH STAYS VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
 * (ldr vptr / ldr [vptr,#0x14] / blx) and a qualified call would silently
 * devirtualise a subclass override.
 *
 * ---- 2. TWO MMIO STORES THAT LAND NOWHERE --------------------------------
 *
 * The function packs two light vectors and stores both to 0x040004c8
 * (LIGHT_VECTOR), and 0x040004c8 is in its own literal pool at 0x020c19c0, so
 * this is the ROM's own address and not a decompilation artifact. Compiled
 * straight from src those stores land in the real memory ntr maps across the
 * I/O window and stop there: the geometry engine is never told, so both lights
 * keep direction (0,0,0) and every lit vertex resolves to emission. That is
 * the "black silhouette" report run mg5 lane YTEX measured on the flower, and
 * tools/hostgen.py exists for it -- but hostgen rewrites a src TU, and a TU
 * that ALSO needs the vtable repair above cannot be a hostgen substitution.
 * So both fixes live here and the stores go through NTR_MMIO, which is what
 * MgFlower_ModelRender_020c3bf4.cpp does for the identical reason.
 *
 * ---- WHAT CHANGED FROM src, so it can be checked line by line -------------
 *
 *   1. the local `struct Obj` shadow is gone, and its two `f5(0)` calls are
 *      `((BlendModelAnim *)(c + 0x1c))->Render(0)` and
 *      `((Model *)(c + 0x8c))->Render(0)`
 *   2. the two `*(volatile unsigned int *)0x40004c8 = ...` stores are
 *      `NTR_MMIO(unsigned int, 0x40004c8) = ...`, the same WORD by a path the
 *      geometry engine sees
 *
 * Nothing else. The two Vector3 initialisations and their 0xfffff008 z, the
 * -0x1000 and 0x1000 rotations, the +0x1e4 angle, both packed-vector
 * expressions with their >> 3 and 0x3ff masks and 10/20 shifts, the
 * 0x40000000 flag on the second, the 0x7fff and 0x4210 calls on +0x1c, the
 * three-bit colour loop off +0x1e2 with its 0x1f/0x14 arms, all three
 * func_020169d8 calls with their 0/4/5 indices and 0x6318 constant, the
 * func_ov006_020c07a0(c + 0xdc) between the two draws and the closing
 * ShadowModel::RenderAll() are src's, transcribed unchanged.
 *
 * IT DEFINES func_ov006_020c1804, so src/func_ov006_020c1804.cpp is OUT of
 * port/slice_mem.txt -- listing both would be an LNK2005.
 *
 * THE DECOMP-SIDE FIX IS NOT ROUTED, because there is nothing wrong with the
 * decomp: the shadow class produces the ROM's own instruction sequence and the
 * store is the ROM's own store. Both defects are the HOST's, which is what
 * makes a port host copy the whole of the remedy.
 */
#include "common.h"
#include "Model.h"
#include "ModelAnim.h"
#include "BlendModelAnim.h"
#include "ntr/mmio.h"      /* NTR_MMIO -- see section 2 */

extern "C" {
extern void func_0203cd80(struct Vector3 *v, int a);
extern void func_0203ccd4(struct Vector3 *v, short angle);
extern void func_02016a14(void *p, int a);
extern void func_02016a04(void *p, int a);
extern void func_020169d8(char *p, int i, unsigned int v);
extern void func_ov006_020c07a0(char *c);
void func_ov006_020c1804(void *self);
}

namespace ShadowModel { void RenderAll(); }

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's)
// plus two I/O-window stores that need the geometry proxy.
void func_ov006_020c1804(void *self)
{
    char *c = (char *)self;
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

    ang = *(short *)(c + 0x1e4);
    func_0203ccd4(&v2, ang);

    p1 = (((short)v1.x >> 3) & 0x3ff)
       | ((((short)v1.y >> 3) & 0x3ff) << 10)
       | ((((short)v1.z >> 3) & 0x3ff) << 20);
    NTR_MMIO(unsigned int, 0x40004c8) = p1;              /* DELTA 2 */

    p2 = (((short)v2.x >> 3) & 0x3ff)
       | ((((short)v2.y >> 3) & 0x3ff) << 10)
       | ((((short)v2.z >> 3) & 0x3ff) << 20);
    NTR_MMIO(unsigned int, 0x40004c8) = p2 | 0x40000000;  /* DELTA 2 */

    func_02016a14(c + 0x1c, 0x7fff);
    func_02016a04(c + 0x1c, 0x4210);

    ang = *(short *)(c + 0x1e2);
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

    ((BlendModelAnim *)(c + 0x1c))->Render(0);           /* DELTA 1 */
    func_ov006_020c07a0(c + 0xdc);
    ((Model *)(c + 0x8c))->Render(0);                    /* DELTA 1 */

    ShadowModel::RenderAll();
}
