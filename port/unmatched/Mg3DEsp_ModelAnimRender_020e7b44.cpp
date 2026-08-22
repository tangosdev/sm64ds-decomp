/* HOST COPY of src/func_ov006_020e7b44.cpp -- the RENDER half of the tick on
 * the sub-object dScMg3DEsp_c holds at this+0x4fd8, reached from vtable slot 9
 * through src/func_ov006_020e9d1c.cpp.  Run mg9, lane PSY.
 *
 * ONE DEFECT, AND IT IS THE ONE hal/cxxname_bridge.cpp PREDICTED IN WRITING.
 * That file's ModelAnim2 block ends:
 *
 *     No dual-fill here: Render's ROM slot (5) is Virtual18's MSVC slot, so
 *     shadow-TU Render dispatch cannot be served by the same array --
 *     trap-by-Virtual18 will name it if such a TU ever appears.
 *
 * SUCH A TU HAS APPEARED.  This is it, and it was found by the check run mg9's
 * coordinator made mandatory after lane S381 -- ask what the ROM parks in the
 * first word of every object the factory builds -- rather than by a fault.
 *
 * ---- 1. WHAT src DOES ----------------------------------------------------
 *
 *     struct Obj { virtual void v0(); ... virtual void m5(int a); };
 *     ((Obj*)(c + 0xc))->m5(0);
 *
 * a LOCAL SIX-VIRTUAL SHADOW, so `m5` is index 5, byte +0x14.  That is the ROM's
 * own numbering (Itanium spends two slots on the destructor) and the ROM agrees
 * exactly, read out of extracted/overlays/overlay_0006.bin at base 0x020bfec0:
 *
 *     020e7bc0  add r0,r4,#0xc        the object at the sub-object's +0xc
 *     020e7bc4  ldr r2,[r0]           its vptr
 *     020e7bcc  ldr r2,[r2,#0x14]     ROM slot 5
 *     020e7bd0  blx r2                with r1 = 0
 *
 * ---- 2. WHICH CLASS THE OBJECT IS, FROM THE CONSTRUCTOR ------------------
 *
 * The memory2 ruling: establish the class from the constructor, never from the
 * call.  src/func_ov006_020e814c.c builds this sub-object and runs
 * `_ZN9ModelAnimC1Ev(c + 0xc)` and `_ZN15TextureSequenceC1Ev(c + 0x70)`, and
 * src/func_ov006_020e80d8.cpp tears the same two down with
 * _ZN9ModelAnimD1Ev and _ZN15TextureSequenceD1Ev.  So +0xc is a PLAIN
 * ModelAnim -- not a ModelAnim2 and not a BlendModelAnim.
 *
 * ---- 3. WHY THE DUAL FILL DOES NOT SAVE THIS ONE -------------------------
 *
 * hal_fill_model_vtable() writes _ZTV5Model[4] AND _ZTV5Model[5] both to
 * mv_render precisely so a shadow-class Render dispatch on a Model lands right,
 * and slot 9 of this class relies on that for its two Models at +0x4f38 and
 * +0x4f88.  hal_fill_modelanim2_vtable() DELIBERATELY DOES NOT do the same for
 * _ZTV9ModelAnim, because that array's slot 5 is a real body:
 *
 *     _ZTV9ModelAnim[4] = ma2_render      Render(const Vector3 *scale)
 *     _ZTV9ModelAnim[5] = ma2_virtual18   Virtual18(u32 mat, const Vector3 *)
 *
 * so the shadow call reaches Virtual18 with one argument where it takes two.
 * `mat` arrives 0 and Virtual18's first statement is Virtual10(*(Matrix4x3*)mat)
 * -- a matrix reference at address zero.  That is the Y3D chain
 * unmatched/MgFlower_ModelRender_020c3bf4.cpp and
 * unmatched/MgMemory2_ModelRender_020c1804.cpp both record, ending in the same
 * `movups xmm0,[eax]` with eax zero.
 *
 * NOT MEASURED AS A FAULT, AND THAT IS STATED RATHER THAN GLOSSED.  This lane's
 * boots run with SM64DS_SCENE_NO_RENDER=1 for the reason every headless proof
 * run does, so slot 9 does not execute and this site is not entered.  The
 * evidence is the ROM instruction at 0x020e7bcc, the constructor at
 * 0x020e814c and the two vtable fills above -- three static reads that agree.
 * A rendered run is what would turn it into a fault, and the repair is here so
 * that run does not have to.
 *
 * ---- 4. THE DELTA FROM src, line by line ---------------------------------
 *
 *   1. the local `struct Obj` shadow is gone and its `m5(0)` call is
 *      `((ModelAnim *)(c + 0xc))->Render(0)`
 *
 * Nothing else.  THE DISPATCH STAYS VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
 * (ldr vptr / ldr [vptr,#0x14] / blx) and a qualified call would silently
 * devirtualise a subclass override.  The idle-sentinel test against
 * data_ov006_0213c744 above it, the func_02046208 call with the byte at +0x21a,
 * the `<< 0x11 >> 4` on the halfword at +0x218 into +0x78, the
 * TextureSequence::Update at +0x70 over the ModelComponents at +0x14, and the
 * func_ov006_020e7b0c and func_ov006_020e777c calls are src's, unchanged.
 *
 * THE SENTINEL STAYS AS TWO INTS.  This body compares the eight-byte member
 * pointer at +0x210 against data_ov006_0213c744 BY VALUE and returns early on a
 * match; it does not dispatch it.  unmatched/Mg3DEsp_FieldPmf.cpp is the half
 * that does.  Nothing here rewrites the stored pair, which is what keeps this
 * comparison answering what the ROM answers.
 *
 * IT DEFINES func_ov006_020e7b44, so src/func_ov006_020e7b44.cpp is OUT of
 * port/slice_psy.txt -- listing both would be an LNK2005.
 *
 * THE DECOMP-SIDE FIX IS NOT ROUTED, because there is nothing wrong with the
 * decomp: the shadow class produces the ROM's own instruction sequence. The
 * defect is the HOST's, which is what makes a host copy the whole of the
 * remedy.
 */

#include "ModelAnim.h"

struct ModelComponents;

struct TextureSequence {
    void Update(ModelComponents &m);
};

extern "C" {
void func_02046208(char *a, int b, int c);
void func_ov006_020e7b0c(char *c);
void func_ov006_020e777c(char *c);
extern int data_ov006_0213c744[];
}

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's)
extern "C" void func_ov006_020e7b44(char *c)
{
    int *p = (int *)(((int)c + 0x210));
    int *d = data_ov006_0213c744;
    if (p[0] == d[0]) {
        if (p[1] == d[1]) return;
        if (*(int *)(c + 0x210) == 0) return;
    }

    func_02046208(c + 0x14, *(unsigned char *)(c + 0x21a), 0);

    *(int *)(c + 0x78) = (unsigned)((*(short *)(c + 0x218)) << 0x11) >> 4;
    ((TextureSequence *)(c + 0x70))->Update(*(ModelComponents *)(c + 0x14));

    func_ov006_020e7b0c(c);

    ((ModelAnim *)(c + 0xc))->Render(0);                 /* DELTA 1 */

    func_ov006_020e777c(c + 0x84);
}
