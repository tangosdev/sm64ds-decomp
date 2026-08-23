/* HOST COPIES of TWO dScMgD3DBase_c sub-object bodies -- func_ov006_020c4c54
 * and func_ov006_020c7734 -- for the MSVC-vs-ROM VTABLE NUMBERING defect
 * hal/cxxname_bridge.cpp's ModelAnim2 block predicted in writing and run mg9
 * lane PSY hit twice.  Run mg11, lane BNT.
 *
 * NOT NAMED AFTER A LANE, for unmatched/MgD3DBase_Slots.cpp's reason.  Both
 * bodies belong to the two sub-object families dScMgD3DBase_c's derived
 * classes build, and the two array constructors are reached from BOTH 0x174's
 * factory and 0x175's -- the only relocations to func_ov006_020c6f70 (the
 * +0x5234 element ctor) are from:0x020eebe4 and from:0x020efbf4, and the only
 * two to func_ov006_020c8a04 (the +0x500c one) are from:0x020eebd8 and
 * from:0x020efbe8.  Lane BNP needs both repairs; ONE COPY SURVIVES the merge.
 *
 * ---- 1. THE DEFECT, AND IT WAS A REAL FAULT -----------------------------
 *
 * Both bodies dispatch a LOCAL SIX-VIRTUAL SHADOW's `m5` on an object that is
 * a plain ModelAnim.  `m5` is index 5, byte +0x14, which is the ROM's own
 * Itanium numbering (two slots for the destructor) and is what the ROM does:
 *
 *   func_ov006_020c4c54                func_ov006_020c7734
 *     020c4cb0 add r0,r4,#0x38           020c7828 add r0,r5,#0x4c
 *     020c4cb4 ldr r2,[r0]               020c782c ldr r2,[r0]
 *     020c4cb8 ldr r2,[r2,#0x14]         020c7834 ldr r2,[r2,#0x14]
 *     020c4cbc blx r2                    020c7838 blx r2
 *
 * hal_fill_modelanim2_vtable() DELIBERATELY does not dual-fill _ZTV9ModelAnim
 * the way hal_fill_model_vtable() dual-fills _ZTV5Model[4] and [5], because
 * ModelAnim's slot 5 is a real body of its own:
 *
 *     _ZTV9ModelAnim[4] = Render(const Vector3 *scale)
 *     _ZTV9ModelAnim[5] = Virtual18(u32 mat, const Vector3 *scale)
 *
 * so on MSVC the shadow call reaches Virtual18 with one argument where it
 * takes two, `mat` arrives as whatever was next on the stack, and Virtual10's
 * `*(Matrix4x3 *)mat` reads it.
 *
 * THIS ONE IS NOT A PREDICTION.  Run mg9 lane PSY's two instances were found
 * statically and never entered, because every headless proof run there used
 * SM64DS_SCENE_NO_RENDER=1.  Scene 373's BATTERY row renders, and it faulted:
 *
 *     selftest scene 373: FAIL rc=3221225477
 *     FAULT code c0000005 at +0x000a4fa4 accessing 60fff61d
 *     +0x000a4fa4 -> _func_ov006_020c7734 +0x14
 *     walker node 307FA5C0 actor 307FA588 id 0x175
 *     stack: j2_render -> func_ov006_020ef148 -> func_ov006_020c70d0
 *            -> func_ov006_020c7734
 *
 * The 300-frame rendered run is what reaches it; a 60-frame one does not,
 * because the B-family record count is zero until the game puts something in
 * play.  That is worth one line for the next lane: A NO_RENDER PROOF CANNOT
 * SEE THIS CLASS OF DEFECT AT ALL, and a short rendered run can miss it too.
 *
 * ---- 2. WHICH CLASS THE OBJECTS ARE, FROM THE CONSTRUCTORS --------------
 *
 * The memory2 ruling: establish the class from the constructor, never from the
 * call.
 *   src/func_ov006_020c6f70.c   builds the +0x5234 element and runs
 *                               _ZN9ModelAnimC1Ev(t + 0x38)
 *   src/func_ov006_020c8a04.c   builds the +0x500c element and runs
 *                               _ZN9ModelAnimC1Ev((char *)t + 0x4c)
 * Both are PLAIN ModelAnims -- not ModelAnim2 and not BlendModelAnim -- and
 * the offsets are the ones the two dispatch sites add.
 *
 * ---- 3. THE DELTA FROM src, line by line --------------------------------
 *
 *   1. func_ov006_020c4c54: the local `struct Sub` shadow is gone and
 *      `s->m5(&v)` is `((ModelAnim *)(this_ + 0x38))->Render(&v)`.
 *   2. func_ov006_020c7734: the local `struct VBase` shadow is gone and
 *      `((VBase *)(self + 0x4c))->m5(&data_ov006_0212ddd0)` is
 *      `((ModelAnim *)(self + 0x4c))->Render((const Vector3 *)
 *       &data_ov006_0212ddd0)`.
 *
 * Nothing else.  THE DISPATCHES STAY VIRTUAL rather than qualified, for
 * unmatched/MgFlower_ModelRender_020c3bf4.cpp's reason: the ROM's is
 * (ldr vptr / ldr [vptr,#0x14] / blx) and a qualified call would silently
 * devirtualise a subclass override.
 *
 * THE SENTINEL IN func_ov006_020c4c54 STAYS AS TWO INTS.  That body compares
 * the eight-byte member pointer at +0x30 against data_ov006_0213af98 BY VALUE
 * and returns early on a match; it does not dispatch it.
 * unmatched/MgJump2_SubDispatch.cpp is the half that does.  Nothing here
 * rewrites the stored pair, which is what keeps the comparison answering what
 * the ROM answers.
 *
 * BOTH DEFINE THEIR src SYMBOL, so src/func_ov006_020c4c54.cpp and
 * src/func_ov006_020c7734.cpp are OUT of port/slice_bnt.txt -- listing either
 * would be an LNK2005.
 *
 * THE DECOMP-SIDE FIX IS NOT ROUTED, because there is nothing wrong with the
 * decomp: the shadow classes produce the ROM's own instruction sequence. The
 * defect is the HOST's, which is what makes a host copy the whole of the
 * remedy.
 */

#include "ModelAnim.h"

struct OamAttr;
struct Matrix2x2;

extern "C" {

void Vec3_MulScalar(Vector3 *res, Vector3 *v, int scalar);
void func_ov006_020bfec0(void *a, void *b, short *c);
int  func_02053200(int x);
void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(
        bool en, OamAttr *attr, int a, int b, int c, int d, Matrix2x2 *m);

extern Vector3 data_ov006_0213af98;
extern unsigned char data_ov006_02140400;
extern void *data_ov006_02141a44;
extern unsigned short data_ov006_02140404;
extern short data_02082214[];
extern OamAttr *data_ov006_02134d1c;
extern char data_ov006_0212ddd0;

}  /* extern "C" */

struct Mtx { int a, b, c, d; };

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's)
extern "C" void func_ov006_020c4c54(int this_)
{
    Vector3 v;
    int *p = (int *)(((int)this_ + 0x30));
    Vector3 *d = &data_ov006_0213af98;
    if (p[0] == d->x) {
        if (p[1] == d->y)
            return;
        if (((volatile int *)this_)[0xc] == 0)
            return;
    }
    Vec3_MulScalar(&v, (Vector3 *)(this_ + 0xcc), *(int *)(this_ + 0xd8));
    ((ModelAnim *)(this_ + 0x38))->Render(&v);            /* DELTA 1 */
}

// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's)
extern "C" void func_ov006_020c7734(char *self)
{
    short v[2];
    int r1res;
    int r2res;
    int g;
    int t;
    Mtx m;

    if (*(unsigned char *)(self + 0x35) == 0)
        return;

    if (data_ov006_02140400 != 0) {
        func_ov006_020bfec0(data_ov006_02141a44, self + 0x14, v);

        g = data_ov006_02140404;
        t = data_02082214[(g >> 4) * 2];
        v[1] = (short)(v[1] - (((t << 2) + 0x30000) >> 12));
        r1res = func_02053200((t >> 2) + 0x1000);

        g = data_ov006_02140404;
        r2res = -func_02053200((data_02082214[(g >> 4) * 2 + 1] >> 2) + 0x1000);

        int *mp = (int *)&m;
        int z = 0;
        mp[0] = z; mp[1] = z; mp[2] = z; mp[3] = z;
        m.d = r2res;
        m.a = r1res;
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(
            false, data_ov006_02134d1c, v[0], v[1], -1, -1, (Matrix2x2 *)&m);
    }

    ((ModelAnim *)(self + 0x4c))->Render(                 /* DELTA 2 */
        (const Vector3 *)&data_ov006_0212ddd0);
}
