/* THE TITLE DOODLE CENSUS, ONE LINE PER FRAME.  DEFAULT OFF.
 *
 * run push0829 lane TITLE-DOODLE. SM64DS_DOODLE_PROBE=1.
 *
 * The doodle (the DS lets you draw on the bottom screen with the stylus at the
 * title) is a SEPARATE spawned actor whose handle the ROM parks in
 * data_ov007_02104ba0, allocated 0x4c bytes by src/func_ov007_020b9880.c when the
 * title switches to the draw screen (src/func_ov007_020b0358.c case 2 calls
 * func_ov007_020bd1c0(1) which reaches the constructor). Until that construction
 * runs the handle is null, so this probe's first job is to say whether the actor
 * EXISTS at all on a given run.
 *
 * The stroke storage the draw hub func_ov007_020b91b4 reads (and clears) is on
 * that object: the stroke-count word at +0xac, the pending-point word at +0xb0,
 * and the capacity at +0x8. The doodle mode word is at +0x20 ([8] as ints). This
 * probe reports all four plus the anim-state object's state word (+0x8 -> [0]),
 * so a run can be told apart three ways: the actor never spawned, it spawned but
 * no stroke was recorded, or a stroke was recorded and the count moved.
 *
 * It exists because "the doodle did not draw" has those three distinct causes and
 * a framebuffer capture separates none of them, exactly the reason the sibling
 * SM64DS_SCENE_TOUCH_TRACE and SM64DS_TITLE_ATTRACT_PROBE exist. Two integer
 * compares when the variable is unset. Its one call site is in
 * hal/scene_boot.cpp's port_scene_tick, after port_actor_tick, so the fields read
 * are this frame's.
 */
#include <cstdio>
#include <cstdlib>

extern "C" int* data_ov007_02104ba0;   /* the doodle actor handle */
extern "C" int* data_ov007_0210342c;   /* the title scene global  */

extern "C" void port_title_doodle_probe(int frame, const char* when)
{
    static int on = -1;
    if (on < 0) {
        const char* e = std::getenv("SM64DS_DOODLE_PROBE");
        on = e ? std::atoi(e) : 0;
        if (e && on == 0) on = 1;   /* any non-numeric value means level 1 */
    }
    if (!on) return;

    int* d = data_ov007_02104ba0;
    if (!d) {
        std::fprintf(stderr, "[doodle] %s f%-5d actor=NULL\n", when, frame);
        std::fflush(stderr);
        return;
    }

    /* THE STROKE STORAGE IS ON THE VIEW OBJECT AT d[0] (field +0x0), NOT on the
       0x4c-byte actor.  src/func_ov007_020b91b4.c reads r6 = *(char**)02104ba0 and
       then a0[0xac]/a0[0xb0]; src/func_ov007_020c4dfc.c reads c+0xac (stroke
       count), c+8 (capacity), c+0x28 (point array), c+0x38 (per-stroke header
       array) with c = *(02104ba0).  So the counters live one indirection down. */
    const int* view = (const int*)d[0];                   /* +0x00 view object */
    const char* animobj = (const char*)d[2];              /* +0x08 anim-state    */
    const int animstate = animobj ? (int)*(const short*)animobj : -999;
    const int mode      = d[8];                           /* +0x20 doodle mode   */
    int strokes = -1, pending = -1, cap = -1;
    if (view) {
        cap     = view[8 / 4];                            /* +0x08 capacity      */
        strokes = view[0xac / 4];                         /* +0xac stroke count  */
        pending = view[0xb0 / 4];                         /* +0xb0 pending point */
    }

    /* the title's stylus tracker, so a doodle line can be read next to the touch
       it was fed: g+0x50 -> {now @ +0xc, prev @ +0x14, released @ +0x24}. */
    const char* g = (const char*)data_ov007_0210342c;
    const char* trk = g ? *(const char* const*)(g + 0x50) : 0;
    int now = -1, prev = -1, rel = -1;
    if (trk) {
        now  = (int)*(const unsigned short*)(trk + 0x0c);
        prev = (int)*(const unsigned short*)(trk + 0x14);
        rel  = (int)*(const int*)(trk + 0x24);
    }

    std::fprintf(stderr,
                 "[doodle] %s f%-5d actor=%p view=%p animstate=%d mode=%d cap=%d "
                 "strokes=%d pending=%d  trk{now=%d prev=%d rel=%d}\n",
                 when, frame, (void*)d, (void*)view, animstate, mode, cap,
                 strokes, pending, now, prev, rel);
    std::fflush(stderr);
}
