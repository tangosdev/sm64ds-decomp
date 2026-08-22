/* HOST COPY of src/func_ov006_020c29dc.cpp -- the per-frame model draw of
 * dScMgCup_c's 3D sub-object at +0x4f38 (the table, the cups and the ball).
 * Run mg9, lane CUP.
 *
 * WHAT IT FIXES, and it is the LOCAL SHADOW CLASS problem
 * port/unmatched/MgFlower_ModelRender_020c3bf4.cpp and
 * port/unmatched/MgMemory2_ModelRender_020c1804.cpp already name, one class
 * over and at TWO call sites rather than one. hal/cxxname_bridge.cpp fills the
 * host model vtables in MSVC order, where the destructor costs ONE slot and
 * Itanium's costs two:
 *
 *   _ZTV5Model           dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3,
 *                        Render 4
 *   _ZTV14BlendModelAnim dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3,
 *                        Render 4, Virtual18 5
 *
 * src/func_ov006_020c29dc.cpp models BOTH sub-objects with one local
 * `struct Obj` of six virtuals plus a leading `void *vt` member and calls
 * `m(...)`, its sixth virtual. That is the ROM's own numbering: BYTE +0x14 of
 * an Itanium vtable is the sixth word, and on both model classes that word is
 * Render. Read straight off extracted/overlays/overlay_0006.bin at base
 * 0x020bfec0:
 *
 *   020c2b30  add  r0,r6,#8              ; the BlendModelAnim at +8
 *   020c2b3c  ldr  r2,[r0]               ; vptr
 *   020c2b40  mov  r1,#0                 ; scale = NULL
 *   020c2b44  ldr  r2,[r2,#0x14]         ; ROM slot 5 -> Render
 *   020c2b48  blx  r2
 *   020c2b4c  add  r0,r6,#0x78           ; the Model at +0x78
 *   020c2b50  ldr  r2,[r0]
 *   020c2b54  ldr  r1,[pc,#0x2c]         ; = 0x0212b8fc
 *   020c2b58  ldr  r2,[r2,#0x14]         ; ROM slot 5 -> Render
 *   020c2b5c  blx  r2
 *
 * and data_ov006_0212b8fc reads { 0x1600, 0x1600, 0x1000 } -- a 20.12 Vector3
 * scale of 1.375, 1.375, 1.0, which is what Render's one parameter is. A
 * second, independent reading that says slot 5 is Render here: the four lines
 * above the calls adjust the ModelComponents' own +0x104/+0x108/+0x134/+0x138
 * words and then the object is drawn, which is coherent for Render and is not
 * coherent for Virtual18.
 *
 * ON THE HOST that same byte offset is the SIXTH MSVC word. On BlendModelAnim
 * that is Virtual18(u32 mat, const Vector3 *scale); the shadow class passes
 * one argument, so mat arrives 0, and Virtual18's first statement is
 * Virtual10(*(Matrix4x3 *)mat) -- a matrix reference at address zero. On Model
 * there is no sixth word at all.
 *
 * MEASURED, scene 361, this tree, the first run with the render slot live:
 *
 *   walk_window crash  code c0000005  access 00000000 at 00000000
 *   +000a411c -> Model::Virtual10 +0xc
 *   +0013cfa2 -> BlendModelAnim::Virtual10 +0x42
 *   +000a415e -> ModelAnim::Virtual18 +0xe
 *   +0008639c -> func_ov006_020c29dc +0x15c
 *   +00087db0 -> func_ov006_020e0068 +0x140      (vtable slot 9, Render)
 *   +00084ffc -> cup_render +0xc
 *
 * eax 00000000, reading address 0 -- the null Matrix4x3, the same landing spot
 * lane Y3D measured on the flower and lane MEM measured on Memory Master.
 *
 * THE TWO MMIO STORES HAVE TO GO THROUGH THE PROXY, and that is the second
 * repair rather than a style choice. This is a host copy, so
 * tools/hostgen.py never sees it, and hostgen is what rewrites
 * `*(volatile int *)0x40004cc` into NTR_MMIO for the matched tree. A plain
 * store lands in the real memory ntr maps across the I/O window and stops
 * there -- the geometry engine is never told, LIGHT_COLOR stays whatever the
 * last scene left and the model draws with the wrong light. The stored WORDS
 * are the matched source's, unchanged; only the path they take to the engine
 * is. Both literals are in the ROM's own pool: 0x020c2b74 = 0x00007fff,
 * 0x020c2b78 = 0x040004cc, 0x020c2b80 = 0x040004c8, 0x020c2b84 = 0x4000294b.
 *
 * THE DISPATCH STAYS VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
 * (ldr vptr / ldr [vptr,#0x14] / blx), and a qualified call would silently
 * devirtualise a subclass override.
 *
 * WHAT CHANGED FROM src, so it can be checked line by line: the local
 * `struct Obj` shadow and its two `m(...)` calls became virtual Render calls
 * on BlendModelAnim and Model, and the two raw I/O stores became NTR_MMIO.
 * Every other statement, constant, offset and packing expression is the
 * matched source's, transcribed unchanged.
 *
 * THE COST IS ONE LINKED FUNCTION. src/func_ov006_020c29dc.cpp is out of
 * port/slice_cup.txt and port/tools/linkage.py stops counting it, the same
 * trade the two precedent files made. src/func_ov006_0211c6c4.c also calls
 * this body, for a class this port has not seated; it gets the repaired
 * version for free when it is.
 */
#include "common.h"
#include "Model.h"
#include "ModelAnim.h"
#include "BlendModelAnim.h"
#include "ntr/mmio.h"

extern "C" {
extern void func_02016a04(void *p, int a);
extern void func_0203cd80(int *m, short angle);
extern void func_0203ccd4(int *m, short angle);
extern void _ZN15TextureSequence6UpdateER15ModelComponents(void *ts, void *mc);
extern void _ZN18TextureTransformer6UpdateER15ModelComponents(void *tt, void *mc);
extern char data_ov006_0212b8fc;
void func_ov006_020c29dc(char *c);
}

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's).
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

    /* LIGHT_COLOR then LIGHT_VECTOR, twice -- light 0 then light 1. See the
       header for why these are NTR_MMIO here and plain casts in src. */
    NTR_MMIO(int, 0x40004cc) = 0x7fff;
    NTR_MMIO(int, 0x40004c8) =
        (((short)v0[0] >> 3) & 0x3ff) |
        ((((short)v0[1] >> 3) & 0x3ff) << 10) |
        ((((short)v0[2] >> 3) & 0x3ff) << 20);
    mc = c + 0x10;
    NTR_MMIO(int, 0x40004cc) = 0x4000294b;
    NTR_MMIO(int, 0x40004c8) =
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

    /* ROM slot 5 on both, byte +0x14 of the Itanium table: Render. */
    ((BlendModelAnim *)(c + 8))->Render(0);
    ((Model *)(c + 0x78))->Render((const Vector3 *)&data_ov006_0212b8fc);
}
