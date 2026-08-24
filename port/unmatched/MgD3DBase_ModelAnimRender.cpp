/* PORT_HOST_ABI. TWO instances of the mwcc VIRTUAL-SHADOW DISPATCH defect, in
 * the two per-frame sub-object draws dScMgJump_c's Render slot reaches.
 * Run mg11, lane BNP.
 *
 * SHARED FILE, NOT A LANE FILE. Both bodies live in ov006's shared
 * minigame-object region and every dScMgD3DBase_c subclass -- 0x174, 0x175,
 * 0x180 and 0x181 -- draws through them. Named for the base, like
 * unmatched/MgD3DBase_Slot2.cpp and unmatched/MgD3DBase_ObjStateDispatch.cpp.
 *
 * ---- WHAT IS WRONG, AND IT IS THE DEFECT MgMemory2_ModelRender_020c1804.cpp
 * AND MgFlower_ModelRender_020c3bf4.cpp AND Door_Render.cpp ALREADY RECORD ---
 *
 * hal/cxxname_bridge.cpp fills the host model vtables in MSVC order, where the
 * destructor costs ONE slot and Itanium's costs two:
 *
 *   _ZTV9ModelAnim   dtor 0, DoSetFile 1, UpdateVerts 2, Virtual10 3,
 *                    Render 4, Virtual18 5
 *
 * and that file says in as many words that _ZTV9ModelAnim CANNOT be
 * dual-filled the way _ZTV5Model is, because Virtual18 really is at its slot 5.
 *
 * Both src TUs spell their sub-object as a local SIX-PURE-VIRTUAL struct and
 * call the sixth method, which is the ROM's own numbering: byte +0x14 of an
 * Itanium vtable is the sixth word, and on ModelAnim that word is Render. Read
 * straight off extracted/overlays/overlay_0006.bin at base 0x020bfec0, both
 * sites are the same three instructions:
 *
 *   func_ov006_020c4c54, the object at this+0x38
 *     ...  ldr r2,[r0] / ldr r2,[r2,#0x14] / blx r2 with r1 = &scratch Vector3
 *
 *   func_ov006_020c7734, the object at this+0x4c
 *     ...  ldr r2,[r0] / ldr r2,[r2,#0x14] / blx r2 with r1 = &data_ov006_0212ddd0
 *
 * and BOTH objects are plain ModelAnims: src/func_ov006_020c6f70.c constructs
 * one with _ZN9ModelAnimC1Ev(t + 0x38) and src/func_ov006_020c8a04.c the other
 * with _ZN9ModelAnimC1Ev(t + 0x4c). So on the host the shadow's sixth word is
 * Virtual18(unsigned mat, const Vector3 *scale) rather than
 * Render(const Vector3 *scale), the one argument the shadow passes lands in
 * `mat` instead of `scale`, and Virtual18's first statement dereferences mat as
 * a Matrix4x3.
 *
 * MEASURED, scene 372, this tree, AND PROVEN BY SCRATCH REVERT rather than by
 * one observation. The reproducible statement is:
 *
 *   WITHOUT this file (src/func_ov006_020c4c54.cpp and
 *   src/func_ov006_020c7734.cpp back in port/slice_bnp.txt, this file out of
 *   the CMake source list, full rebuild):
 *     SM64DS_SCENE=372 SM64DS_FAULTS_FATAL=1 SM64DS_BNP_START_STATE=1
 *     at 300, 600 and 1200 frames  ->  exit -1073741819 (0xC0000005), 3 of 3
 *
 *   WITH this file, the same three runs  ->  exit 0, 3 of 3
 *
 * The fault record from the reverted build:
 *
 *   FAULT code c0000005 at +0xffc00000 accessing 00000000
 *     walker node 307FA804 actor 307FA7CC id 0x174
 *     regs eax=00000000 ecx=307fa7cc ebx=307fa7cc edi=307fa7cc
 *     stack: main -> port_scene_run -> port_scene_tick -> port_actor_render ->
 *            func_02043fdc -> port_dispatch_guarded -> func_0204322c ->
 *            port_actor_process
 *
 * 307FA7CC + 0x38 = 307FA804, so the walker node is the actor's RENDER list
 * node. func_0204322c is ActorBase::Process dispatching the three arm9
 * member-pointer globals data_02099ecc / data_02099e74 / data_02099e9c, whose
 * ROM words are {0x24,1}, {0x28,1} and {0x2c,1} -- virtual, vtable bytes 0x24,
 * 0x28 and 0x2c, which are slots 9, 10 and 11: Render, BeforeRender,
 * AfterRender. An independent control agrees: the same binary with
 * SM64DS_SCENE_SLOT9=0 -- this class's Render replaced by a counting no-op --
 * runs 60 render frames clean with BeforeRender and AfterRender both entered
 * 60 times, and scenes 386, 388 and 389 render clean on the same binary, so the
 * path is sound and the defect is inside the Render slot.
 *
 * BOTH LOOPS REALLY RUN, which is what puts these two bodies on that path and
 * is measured rather than assumed. func_ov006_020c425c iterates
 * data_ov006_02140328 times over func_ov006_020c4c54 and func_ov006_020c70d0
 * iterates data_ov006_02140418[0] times over func_ov006_020c7734; a probe in
 * the seat's Render thunk reads those two globals on the first frame of a
 * scene-372 boot as 6 and 3.
 *
 * ONE EARLIER OBSERVATION IS RECORDED AND NOT RELIED ON. The first crash this
 * lane saw was a render run with the state machine NOT started, on a binary
 * two builds older, with the same dump shape. That configuration does NOT
 * reproduce on the current tree in either direction -- reverted or repaired, a
 * plain render run of scene 372 exits 0 -- so the case for this file rests on
 * the three-and-three revert pair above and not on that first sighting.
 *
 * ---- WHAT CHANGED FROM src, so it can be checked line by line -------------
 *
 * In each body the local six-virtual struct and its `->m5(arg)` call become a
 * call through the REAL class, which is what makes MSVC pick its own slot 4:
 *
 *   src:   struct Sub { ... virtual void m5(Vector3 *p); };
 *          ((Sub *)(this_ + 0x38))->m5(&v);
 *   here:  ((ModelAnim *)(this_ + 0x38))->Render(&v);
 *
 * Everything else in both TUs is src's, verbatim, including the guards, the
 * offsets and the OAM::Render argument list.
 */

/* ---- THE MERGE ASSEMBLED THIS FILE FROM TWO LANES -------------------------
 *
 * Lanes BNP and TTE of run mg11 each wrote a file of this name, and they are
 * NOT the same file: BNP ruled the two sub-object draws (020c4c54, 020c7734)
 * and TTE ruled two more one object out (020c8e90, 02122814). Four distinct
 * bodies, no overlap, so the merge appends rather than picks -- the comment
 * above is BNP's and the one below is TTE's, both kept because each documents
 * its own pair. The extern block is the union of the two.
 *
 * The lane review that called this file "a union" of the SAME bodies was
 * wrong and was corrected before the merge; it is a union of DISJOINT pairs.
 */

/* PORT_HOST_ABI.  TWO mwcc VIRTUAL-SHADOW DISPATCHES onto ModelAnim's ROM slot
 * 5, in the dScMgD3DBase_c family's render path.  Run mg11.
 *
 * THE FILE NAME IS NOT LANE-SCOPED, for port/unmatched/MgD3DBase_Slots.cpp's
 * reason: the four ids under dScMgD3DBase_c (0x174, 0x175, 0x180, 0x181) share
 * this render machinery, and more than one mg11 lane reaches it.  Compare
 * contents at merge rather than assuming a duplicate.
 *
 * ---- THE DEFECT, WHICH IS THE FLOWER'S ------------------------------------
 *
 * port/unmatched/MgFlower_ModelRender_020c3bf4.cpp states it in full and this
 * file does not restate the whole of it.  In short: the ROM dispatches
 *
 *     ldr r2,[r0]         the vptr
 *     ldr r2,[r2,#0x14]   ROM slot 5 -> ModelAnim::Render(Vector3 const *)
 *     blx r2
 *
 * and the matched sources spell that as a LOCAL SHADOW CLASS with six virtuals,
 * calling the sixth.  On the host that same byte offset is MSVC's sixth word,
 * which on ModelAnim is Virtual18(u32 mat, const Vector3 *scale) -- a different
 * function with a different first parameter.  include/ModelAnim.h:74 declares
 * the real one `virtual void Render(const Vector3 *scale)` at slot 5.
 *
 * ---- HOW IT PRESENTED HERE -----------------------------------------------
 *
 * It is INVISIBLE to SM64DS_SCENE_NO_RENDER=1 and it was invisible to this lane
 * for a second reason on top: until port/unmatched/MgD3DBase_Slots.cpp put the
 * receiver back into dScMgD3DBase_c slot 10, Actor::BeforeRender answered NO on
 * every frame and the framework skipped SLOT 9 -- so no render code ran to be
 * wrong.  (Slot 10 itself WAS dispatched, 1500 times on a 1500-frame rendered
 * boot, and so was slot 11; only slot 9 read zero.  An earlier version of this
 * header said all three read zero, which was a NO_RENDER census quoted in a
 * rendered comparison.)  With slot 10 repaired, a 300-frame RENDERED boot of
 * scene 385 faults immediately:
 *
 *     FAULT code c0000005 at +0x303fd460 accessing 307fd460
 *       regs ... ebp=004915f4   -> func_ov006_020caadc+0xf4
 *       stack[10] +0x00095cab   -> func_ov006_021231ac+0x11b  (vtable slot 9)
 *       stack[16] +0x0008e3ec   -> tte_render
 *       ... port_actor_render / port_scene_tick / port_scene_run
 *
 * and 0x307fd460 is the live scene object 0x307F8454 PLUS 0x500c -- the first
 * Trampoline-Mario record, i.e. the call jumped to its own operand.  That is
 * the signature of a receiver arriving on the stack where the callee expects a
 * register.
 *
 * ---- THE TWO SITES -------------------------------------------------------
 *
 * src/func_ov006_020c8e90.cpp, reached every rendered frame:
 *     struct C { virtual void a0()..a4(); virtual void f(void *); };
 *     ((C *)(o + 0x78))->f((void *)data_ov006_0212e02c);
 * The object is the Trampoline-Mario record's ModelAnim at +0x78 -- the same
 * +0x78 its constructor (src/_ZN6Player29TryExitCharacterDoorWithIntroEv.cpp,
 * misnamed) runs _ZN9ModelAnimC1Ev on.  data_ov006_0212e02c is four words of
 * 0x2000, a Vector3 scale of 2.0 in 20.12, which is what Render's parameter is.
 *
 * src/func_ov006_02122814.cpp, the same shape one object out:
 *     ((C *)c)->v5(c + 0x50);
 * with c's own +0x1c written from data_020a0e68 immediately before -- and
 * Model::mat4x3 is +0x1c into a Model, so `c` is a Model/ModelAnim by its own
 * preceding statement.  The argument c+0x50 is that object's scale vector.
 *
 * THE CLOSURE WAS SWEPT RATHER THAN WAITED ON, and the sweep has to be phrased
 * over the CLOSURE rather than over the slice or it cannot see its own answer.
 * Every TU declaring a local struct with virtuals was scanned for WHICH virtual
 * it calls and at what index.
 *
 *   over the 204 lines of port/slice_tte.txt          18 hits, NONE at index 5
 *     13 at index 4 (byte 0x10) on the Trampoline-Mario element's own five-slot
 *        table 0x0213b2e0, which hal/scene_mg_trampoline2.cpp fills, so those
 *        land on this port's own thunks
 *      1 at index 18 (byte 0x48), the scene's slot-18 dispatch from slot 0
 *      2 at index 0, 1 at index 1, 1 at index 2, on argument objects that are
 *        not ModelAnims
 *   plus the two HOST-COPIED files below   2 hits, both at INDEX 5 (byte 0x14)
 *
 * 18 + 2 = 20 over the closure. An earlier version of this paragraph said
 * "every TU in port/slice_tte.txt ... TWENTY hits", which is false twice over:
 * the slice answers 18, and src/func_ov006_020c8e90.cpp and
 * src/func_ov006_02122814.cpp stopped being slice lines the moment this file
 * took them. The two hits it was quoted for were outside the set it named.
 *
 * ---- WHAT DIVERGES FROM src, EXACTLY --------------------------------------
 *
 * The shadow class and its indexed call become a virtual ModelAnim::Render.
 * Every other statement, offset and constant is the matched source's, in the
 * source's order.  The dispatch stays VIRTUAL rather than qualified, for
 * MgFlower_ModelRender_020c3bf4.cpp's reason: the object is only known to be in
 * the ModelAnim family and a qualified call would devirtualise a subclass
 * override.  Both src TUs are OUT of port/slice_tte.txt: listing either beside
 * these definitions would be an LNK2005.
 *
 * THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN.  Nothing about either src is wrong
 * on ARM.
 */

#include "common.h"
#include "ModelAnim.h"

extern "C" {

void Vec3_MulScalar(Vector3 *res, Vector3 *v, int scalar);
void func_ov006_020bfec0(void *a, void *b, short *c);
int  func_02053200(int x);
void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(bool en, struct OamAttr *attr,
                                              int a, int b, int c, int d,
                                              struct Matrix2x2 *m);

extern Vector3 data_ov006_0213af98;
extern unsigned char  data_ov006_02140400;
extern void          *data_ov006_02141a44;
extern unsigned short data_ov006_02140404;
extern short          data_02082214[];
extern struct OamAttr *data_ov006_02134d1c;
extern char           data_ov006_0212ddd0;

void func_ov006_020c4c54(int this_);
void func_ov006_020c7734(char *self);


/* ---- lane TTE's externs, unioned in at the merge ---- */
extern int data_ov006_0212e02c[];
extern Matrix4x3 data_020a0e68;
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationZ(void *m, short angZ);
void func_ov006_020c8e90(char *o);
void func_ov006_02122814(char *c);
}  /* extern "C" */

struct Mtx { int a, b, c, d; };

/* src/func_ov006_020c4c54.cpp, with DELTA 1 at its one dispatch. */
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
    ((ModelAnim *)(this_ + 0x38))->Render(&v);           /* DELTA 1 */
}

/* src/func_ov006_020c7734.cpp, with DELTA 1 at its one dispatch. */
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

        {
            int *mp = (int *)&m;
            int z = 0;
            mp[0] = z; mp[1] = z; mp[2] = z; mp[3] = z;
        }
        m.d = r2res;
        m.a = r1res;
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(
            false, data_ov006_02134d1c, v[0], v[1], -1, -1,
            (struct Matrix2x2 *)&m);
    }

    ((ModelAnim *)(self + 0x4c))->Render((const Vector3 *)&data_ov006_0212ddd0);
                                                          /* DELTA 1 */
}


// PORT_HOST_ABI: mwcc virtual-shadow dispatch (ROM vtable numbering vs MSVC's).
void func_ov006_020c8e90(char *o)
{
    if (*(short *)(o + 0x6e) == 0)
        return;
    ((ModelAnim *)(o + 0x78))->Render((const Vector3 *)data_ov006_0212e02c);
}

// PORT_HOST_ABI: the same, on the object the three statements above it fill.
void func_ov006_02122814(char *c)
{
    Matrix4x3_FromTranslation(&data_020a0e68, *(int *)(c + 0x5c),
                              *(int *)(c + 0x60), *(int *)(c + 0x64));
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short *)(c + 0x74));
    *(Matrix4x3 *)(c + 0x1c) = data_020a0e68;
    ((ModelAnim *)c)->Render((const Vector3 *)(c + 0x50));
}
