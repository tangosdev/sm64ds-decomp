/* HOST COPY of src/func_ov006_020c29dc.cpp -- the per-frame model draw of the
 * ov006 model sub-object dScMgSound_c holds at this+0x4f38, reached from vtable
 * slot 9 (Render) through src/func_ov006_0211c6c4.c.
 * Run mg9, lane BOX. actor id 0x16f = scene 367.
 *
 * TWO DEFECTS IN ONE TU, and they are the SAME PAIR run mg6 lane MEM recorded
 * for dScMgMemory2_c in port/unmatched/MgMemory2_ModelRender_020c1804.cpp and
 * run mg5 lane Y3D recorded for the flower in
 * port/unmatched/MgFlower_ModelRender_020c3bf4.cpp. Neither is visible to the
 * byte gate, to a link, to a `::*` sweep or to a NO_RENDER boot.
 *
 * THIS SEAT FOUND IT THE WAY THE OTHERS DID: A RUN. Scene 367 boots CLEAN at
 * 300 and 1200 frames with SM64DS_SCENE_NO_RENDER=1 and faults on the first
 * RENDERED frame -- which is why port/tools/battery.py, which renders, was the
 * instrument and the lane's own headless boots were not.
 *
 *   FAULT code c0000005 at +0x000a52bc accessing 00000000, eax=00000000
 *     Model::Virtual10+0xc
 *       <- BlendModelAnim::Virtual18+0xe
 *       <- func_ov006_020c29dc+0x15c
 *       <- func_ov006_0211c6c4+0x40        (vtable slot 9, Render)
 *
 * which is the Y3D chain one class over, ending in the same read of address
 * zero.
 *
 * ---- 1. THE LOCAL SHADOW CLASS, TWICE ------------------------------------
 *
 * hal/cxxname_bridge.cpp fills the host model vtables in MSVC order, where the
 * destructor costs ONE slot and Itanium's costs two:
 *
 *   _ZTV5Model           dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3,
 *                        Render 4
 *   _ZTV14BlendModelAnim dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3,
 *                        Render 4, Virtual18 5
 *
 * src spells both objects with a local `struct Obj` carrying a `void *vt`
 * member and six virtuals, and calls the SIXTH -- which is the ROM's own
 * numbering, BYTE +0x14 of an Itanium vtable, and on both model classes that
 * word is Render. Read straight off extracted/overlays/overlay_0006.bin at
 * base 0x020bfec0:
 *
 *   020c2b30  add  r0,r6,#8
 *   020c2b3c  ldr  r2,[r0]            ; vptr of the BlendModelAnim at +8
 *   020c2b40  mov  r1,#0              ; scale = NULL
 *   020c2b44  ldr  r2,[r2,#0x14]      ; ROM slot 5 -> Render
 *   020c2b48  blx  r2
 *   020c2b4c  add  r0,r6,#0x78
 *   020c2b50  ldr  r2,[r0]            ; vptr of the Model at +0x78
 *   020c2b54  ldr  r1,[pc,#0x2c]      ; pool 0x020c2b88 = 0212B8FC
 *   020c2b58  ldr  r2,[r2,#0x14]      ; ROM slot 5 -> Render
 *   020c2b5c  blx  r2
 *
 * On the host that byte is the SIXTH MSVC word, which on BlendModelAnim is
 * Virtual18(u32 mat, const Vector3 *scale). The shadow class passes ONE
 * argument, so `mat` arrives 0, and Virtual18's first statement is
 * Virtual10(*(Matrix4x3 *)mat) -- a matrix reference at address zero. That is
 * the fault above, one for one with MgMemory2_ModelRender_020c1804.cpp's.
 *
 * WHICH CLASS EACH OBJECT IS, established from the CONSTRUCTOR rather than
 * guessed from the call. func_ov006_020c33dc builds this sub-object:
 *
 *   020c33e4  add r0,r4,#8    / bl 0x020166d4   _ZN14BlendModelAnimC1Ev
 *   020c33ec  add r0,r4,#0x78 / bl 0x02016d58   _ZN5ModelC1Ev
 *   020c33f4  add r0,r4,#0xc8 / bl 0x02015a50   _ZN15TextureSequenceC1Ev
 *   020c33fc  add r0,r4,#0xdc / bl 0x02015950   _ZN18TextureTransformerC1Ev
 *
 * so +8 is a BlendModelAnim and +0x78 is a Model, and include/Model.h and
 * include/BlendModelAnim.h both list `virtual void Render(const Vector3 *)` at
 * slot 5 independently of this reading.
 *
 * THE DISPATCH STAYS VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
 * (ldr vptr / ldr [vptr,#0x14] / blx) and a qualified call would silently
 * devirtualise a subclass override.
 *
 * ---- 2. FOUR MMIO STORES THAT LAND NOWHERE -------------------------------
 *
 * This TU writes the geometry command port TWICE and the light vector TWICE,
 * and both addresses are in its OWN literal pool at 0x020c2b78 (0x040004cc)
 * and 0x020c2b80 (0x040004c8), so they are the ROM's addresses and not a
 * decompilation artefact:
 *
 *   0x040004cc = 0x00007fff      LIGHT_COLOR, light 0 white
 *   0x040004c8 = pack(v0)        LIGHT_VECTOR, light 0
 *   0x040004cc = 0x4000294b      LIGHT_COLOR, light 1
 *   0x040004c8 = pack(v1)|1<<30  LIGHT_VECTOR, light 1
 *
 * Compiled straight from src those stores land in the real memory ntr maps
 * across the I/O window and stop there: the geometry engine is never told, so
 * both lights keep direction (0,0,0) and every lit vertex resolves to
 * emission. That is the "black silhouette" report run mg5 lane YTEX measured
 * on the flower. tools/hostgen.py exists for it, but hostgen REWRITES a src TU
 * and a TU that also needs the vtable repair above cannot be a hostgen
 * substitution -- so both fixes live here, exactly as they do for 020c1804
 * and 020c3bf4.
 *
 * ---- WHAT CHANGED FROM src, so it can be checked line by line -------------
 *
 *   1. the local `struct Obj` shadow is gone, and its two calls are
 *      `((BlendModelAnim *)(c + 8))->Render(0)` and
 *      `((Model *)(c + 0x78))->Render((const Vector3 *)&data_ov006_0212b8fc)`
 *   2. the four `*(volatile int *)0x40004cc / 0x40004c8` stores are
 *      `NTR_MMIO(unsigned int, ...)`, the same WORDS by a path the geometry
 *      engine sees
 *
 * Nothing else. Both Vector3 initialisations and their 0xfffff008 z, the
 * -0x2000/0x2000 and 0x4200/0x7000 rotations, the func_02016a04(c + 8, 0x3d6d)
 * call, both packed-vector expressions with their >> 3 and 0x3ff masks and
 * 10/20 shifts, the 0x40000000 flag on the second, `mc = c + 0x10` and the
 * `p = *(int **)(mc + 4)` read that the ROM performs BEFORE the two Update
 * calls (0x020c2ad4 `ldr r5,[r4,#4]`), both Update calls and all four
 * +0x104/+0x108/+0x134/+0x138 adjustments against +0x194/+0x198/+0x18c/+0x190
 * are src's, transcribed unchanged.
 *
 * IT DEFINES func_ov006_020c29dc, so src/func_ov006_020c29dc.cpp is OUT of
 * port/slice_box.txt -- listing both would be an LNK2005.
 *
 * THE TU IS SHARED WITH dScMgCup_c's CLOSURE. ov006's relocs.txt has exactly
 * two arm_calls to 0x020c29dc: 0x020e01e0 (0x169's) and 0x0211c70c (this
 * class's slot 9). port/slice_mga361.txt lists the src TU and is in NO target,
 * so this seat is the first to build it. If 0x169 is ever seated, ITS lane
 * inherits this host copy rather than writing a second.
 *
 * THE DECOMP-SIDE FIX IS NOT ROUTED, because there is nothing wrong with the
 * decomp: the shadow class produces the ROM's own instruction sequence and the
 * stores are the ROM's own stores. Both defects are the HOST's, which is what
 * makes a port host copy the whole of the remedy.
 */
#include "common.h"
#include "Model.h"
#include "ModelAnim.h"
#include "BlendModelAnim.h"
#include "ntr/mmio.h"      /* NTR_MMIO -- see section 2 */

extern "C" {
extern void func_02016a04(void *p, int a);
extern void func_0203cd80(int *m, short angle);
extern void func_0203ccd4(int *m, short angle);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void *ts, void *mc);
extern void _ZN18TextureTransformer6UpdateER15ModelComponents(void *tt, void *mc);
extern char data_ov006_0212b8fc;
void func_ov006_020c29dc(char *c);
}

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's)
// plus four I/O-window stores that need the geometry proxy.
void func_ov006_020c29dc(char *c)
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

    NTR_MMIO(unsigned int, 0x40004cc) = 0x7fffu;                    /* DELTA 2 */
    NTR_MMIO(unsigned int, 0x40004c8) =                             /* DELTA 2 */
        (unsigned int)((((short)v0[0] >> 3) & 0x3ff) |
                       ((((short)v0[1] >> 3) & 0x3ff) << 10) |
                       ((((short)v0[2] >> 3) & 0x3ff) << 20));
    mc = c + 0x10;
    NTR_MMIO(unsigned int, 0x40004cc) = 0x4000294bu;                /* DELTA 2 */
    NTR_MMIO(unsigned int, 0x40004c8) =                             /* DELTA 2 */
        (unsigned int)((((short)v1[0] >> 3) & 0x3ff) |
                       ((((short)v1[1] >> 3) & 0x3ff) << 10) |
                       ((((short)v1[2] >> 3) & 0x3ff) << 20)) | 0x40000000u;

    p = *(int **)(mc + 4);
    _ZN15TextureSequence6UpdateER15ModelComponents(c + 0xc8, mc);
    _ZN18TextureTransformer6UpdateER15ModelComponents(c + 0xdc, mc);
    *(int *)(((int)p + 0x104)) -= *(int *)(c + 0x194);
    *(int *)(((int)p + 0x108)) += *(int *)(c + 0x198);
    *(int *)(((int)p + 0x134)) += *(int *)(c + 0x18c);
    *(int *)(((int)p + 0x138)) += *(int *)(c + 0x190);

    ((BlendModelAnim *)(c + 8))->Render(0);                          /* DELTA 1 */
    ((Model *)(c + 0x78))->Render((const Vector3 *)&data_ov006_0212b8fc);
                                                                     /* DELTA 1 */
}
