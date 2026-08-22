/* HOST COPY of src/func_ov006_020c29dc.cpp -- the per-frame model draw of the
 * ov006 model sub-object at this+0x4f38, host-copied ONCE for both classes that
 * mount it.
 *
 * Run mg9, adjudicated at the merge. Lane CUP (dScMgCup_c, actor 0x169, scene
 * 361 -- the table, the cups and the ball) and lane BOX (dScMgSound_c, actor
 * 0x16f, scene 367) each host-copied this body into a lane-named file without
 * being able to see the other. THE TWO COPIES WERE BEHAVIOURALLY IDENTICAL;
 * this is the merged text, with both headers' evidence carried. Attribution is
 * in runs/mg9/out/ADJUDICATE/canonical.md.
 *
 * THE BODY IS SHARED, AND THAT IS A ROM FACT. relocs.txt carries EXACTLY TWO
 * arm_calls to 0x020c29dc, one per class, and both are vtable slot 9 (Render):
 *
 *     from:0x020e01e0  inside func_ov006_020e0068   dScMgCup_c   slot 9
 *     from:0x0211c70c  inside func_ov006_0211c6c4   dScMgSound_c slot 9
 *
 * TWO DEFECTS IN ONE TU, and they are the SAME PAIR run mg6 lane MEM recorded
 * for dScMgMemory2_c in port/unmatched/MgMemory2_ModelRender_020c1804.cpp and
 * run mg5 lane Y3D recorded for the flower in
 * port/unmatched/MgFlower_ModelRender_020c3bf4.cpp. Neither is visible to the
 * byte gate, to a link, to a `::*` sweep or to a NO_RENDER boot.
 *
 * BOTH LANES FOUND IT WITH A RUN, INDEPENDENTLY, ONE CLASS APART. Lane BOX's
 * scene 367 boots CLEAN at 300 and 1200 frames with SM64DS_SCENE_NO_RENDER=1
 * and faults on the first RENDERED frame -- which is why port/tools/battery.py,
 * which renders, was the instrument and the lane's own headless boots were not:
 *
 *   FAULT code c0000005 at +0x000a52bc accessing 00000000, eax=00000000
 *     Model::Virtual10+0xc
 *       <- BlendModelAnim::Virtual18+0xe
 *       <- func_ov006_020c29dc+0x15c
 *       <- func_ov006_0211c6c4+0x40        (vtable slot 9, Render)
 *
 * and lane CUP measured the same landing on scene 361, through the other caller:
 *
 *   walk_window crash  code c0000005  access 00000000 at 00000000
 *   +000a411c -> Model::Virtual10 +0xc
 *   +0013cfa2 -> BlendModelAnim::Virtual10 +0x42
 *   +000a415e -> ModelAnim::Virtual18 +0xe
 *   +0008639c -> func_ov006_020c29dc +0x15c
 *   +00087db0 -> func_ov006_020e0068 +0x140      (vtable slot 9, Render)
 *   +00084ffc -> cup_render +0xc
 *
 * Two classes, two scenes, the SAME +0x15c inside this body and the same read
 * of address zero.
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
 * src spells BOTH sub-objects with one local `struct Obj` carrying a `void *vt`
 * member and six virtuals, and calls the SIXTH -- which is the ROM's own
 * numbering, BYTE +0x14 of an Itanium vtable, and on both model classes that
 * word is Render. Read straight off extracted/overlays/overlay_0006.bin at
 * base 0x020bfec0:
 *
 *   020c2b30  add  r0,r6,#8              ; the BlendModelAnim at +8
 *   020c2b3c  ldr  r2,[r0]               ; vptr
 *   020c2b40  mov  r1,#0                 ; scale = NULL
 *   020c2b44  ldr  r2,[r2,#0x14]         ; ROM slot 5 -> Render
 *   020c2b48  blx  r2
 *   020c2b4c  add  r0,r6,#0x78           ; the Model at +0x78
 *   020c2b50  ldr  r2,[r0]
 *   020c2b54  ldr  r1,[pc,#0x2c]         ; pool 0x020c2b88 = 0212B8FC
 *   020c2b58  ldr  r2,[r2,#0x14]         ; ROM slot 5 -> Render
 *   020c2b5c  blx  r2
 *
 * On the host that byte is the SIXTH MSVC word, which on BlendModelAnim is
 * Virtual18(u32 mat, const Vector3 *scale). The shadow class passes ONE
 * argument, so `mat` arrives 0, and Virtual18's first statement is
 * Virtual10(*(Matrix4x3 *)mat) -- a matrix reference at address zero. On Model
 * there is no sixth word at all. That is the fault above, one for one with
 * MgMemory2_ModelRender_020c1804.cpp's.
 *
 * FOUR INDEPENDENT READINGS SAY BYTE +0x14 IS Render HERE, and no two of them
 * share a source. Both lanes are carried because neither is a citation of the
 * other:
 *
 *   (a) THE ROM VTABLES THEMSELVES, out of extracted/arm9_dec.bin at base
 *       0x02004000 -- word 5 of each table:
 *         _ZTV5Model            0x0208e90c  word 5 = 0x02016b78
 *         _ZTV14BlendModelAnim  0x0208e94c  word 5 = 0x020164e4
 *       and config/arm9/symbols.txt names those two
 *       _ZN5Model6RenderEPK7Vector3 and _ZN14BlendModelAnim6RenderEPK7Vector3.
 *   (b) THE CONSTRUCTOR, which says which class each object is rather than
 *       guessing it from the call. func_ov006_020c33dc builds this sub-object:
 *         020c33e4  add r0,r4,#8    / bl 0x020166d4   _ZN14BlendModelAnimC1Ev
 *         020c33ec  add r0,r4,#0x78 / bl 0x02016d58   _ZN5ModelC1Ev
 *         020c33f4  add r0,r4,#0xc8 / bl 0x02015a50   _ZN15TextureSequenceC1Ev
 *         020c33fc  add r0,r4,#0xdc / bl 0x02015950   _ZN18TextureTransformerC1Ev
 *       so +8 is a BlendModelAnim and +0x78 is a Model.
 *   (c) THE ARGUMENT. data_ov006_0212b8fc reads { 0x1600, 0x1600, 0x1000 } --
 *       a 20.12 Vector3 scale of 1.375, 1.375, 1.0, which is exactly what
 *       Render's one parameter is and is not what Virtual18's is.
 *   (d) THE SURROUNDING CODE. The four lines above the calls adjust the
 *       ModelComponents' own +0x104/+0x108/+0x134/+0x138 words and then the
 *       object is drawn, which is coherent for Render and is not coherent for
 *       Virtual18.
 *
 * include/Model.h and include/BlendModelAnim.h both list
 * `virtual void Render(const Vector3 *)` at slot 5 independently of all four.
 *
 * THE DISPATCH STAYS VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
 * (ldr vptr / ldr [vptr,#0x14] / blx) and a qualified call would silently
 * devirtualise a subclass override.
 *
 * ---- 2. FOUR MMIO STORES THAT LAND NOWHERE -------------------------------
 *
 * This TU writes the light colour port TWICE and the light vector TWICE, and
 * both addresses are in its OWN literal pool, so they are the ROM's addresses
 * and not a decompilation artefact. All four pool words, read off the image:
 * 0x020c2b74 = 0x00007fff, 0x020c2b78 = 0x040004cc, 0x020c2b80 = 0x040004c8,
 * 0x020c2b84 = 0x4000294b.
 *
 *   0x040004cc = 0x00007fff      LIGHT_COLOR, light 0 white
 *   0x040004c8 = pack(v0)        LIGHT_VECTOR, light 0
 *   0x040004cc = 0x4000294b      LIGHT_COLOR, light 1
 *   0x040004c8 = pack(v1)|1<<30  LIGHT_VECTOR, light 1
 *
 * Compiled straight from src those stores land in the real memory ntr maps
 * across the I/O window and stop there: the geometry engine is never told, so
 * both lights keep direction (0,0,0) and every lit vertex resolves to emission.
 * That is the "black silhouette" report run mg5 lane YTEX measured on the
 * flower. tools/hostgen.py exists for it, but hostgen REWRITES a src TU and a
 * TU that also needs the vtable repair above cannot be a hostgen substitution
 * -- so both fixes live here, exactly as they do for 020c1804 and 020c3bf4.
 *
 * MEASURED, lane CUP, scene 361, SM64DS_MAT_LOG on 60 frames, reproduced by
 * that lane's reviewer:
 *
 *   LIGHT_VECTOR commands executed: 2   LIGHT_COLOR: 4
 *   light 0 dir=-0.500,-0.707,-0.500 col=1.000,1.000,1.000
 *   light 1 dir= 0.018, 0.996,-0.047 col=0.355,0.323,0.323
 *
 * Nonzero, so the stores reach the engine.
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
 * THE `unsigned int` MMIO SPELLING IS LANE BOX'S AND IS EQUIVALENT TO LANE
 * CUP'S `int`, which is why picking one settled nothing behavioural.
 * NTR_MMIO(T, A) is ::ntr::Reg<T>(A) and Reg<T>::operator=(T v) calls
 * io_write(addr, static_cast<uint64_t>(v), sizeof(T)). sizeof(int) ==
 * sizeof(unsigned int) == 4, so the WIDTH is identical, and all four stored
 * values (0x7fff, the two packed vectors whose highest set bit is 29 and 30,
 * and 0x4000294b) are non-negative as `int`, so the widening never
 * sign-extends. The two lanes emit the same four io_write calls.
 *
 * IT DEFINES func_ov006_020c29dc, so src/func_ov006_020c29dc.cpp is out of
 * port/slice_cup.txt AND out of port/slice_box.txt. Listing it in either would
 * be an LNK2005.
 *
 * ON THE PARAMETER TYPE. include/decl_common.h:2400 declares
 * `extern void func_ov006_020c29dc(void*);` while this definition, like both
 * lanes' copies, spells `char *c`. The symbol has C linkage so the two agree at
 * the link, and the tree has shipped that mismatch green through
 * src/func_ov006_0211c6c4.c (which includes that header) for the whole of lane
 * BOX's battery. Recorded rather than silently changed: `char *` is what the
 * body's own pointer arithmetic needs and what both lanes measured.
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

    /* LIGHT_COLOR then LIGHT_VECTOR, twice -- light 0 then light 1. See the
       header for why these are NTR_MMIO here and plain casts in src. */
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

    /* ROM slot 5 on both, byte +0x14 of the Itanium table: Render. */
    ((BlendModelAnim *)(c + 8))->Render(0);                          /* DELTA 1 */
    ((Model *)(c + 0x78))->Render((const Vector3 *)&data_ov006_0212b8fc);
                                                                     /* DELTA 1 */
}
