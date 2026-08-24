// @symbol _ZN7dWipe_cD1Ev
/* dWipe_c::~dWipe_c() -- complete-object destructor (D1), vtable slot 0, at
 * 0x0202fc08. The D0 without the free.
 *
 * Plain C carrying the literal mangled name, matching sibling convention
 * (src/_ZN10FaderColorD2Ev.c, src/engine/fader/_ZN10FaderColorD1Ev.c): no TU
 * defines dWipe_c's D1 as a real C++ member, so it never collides with the
 * ROM's vtable group. See include/dWipe_c.h and notes/scene-provenance.md.
 */

struct dWipe_c {
    void **vtable;              /* 0x00 */
    char pad[0xf - 4];
    unsigned char needsCleanup; /* 0x0f */
};

extern void *data_020926f0[];                           /* dWipe_c's vtable */
extern void func_0202fb30(struct dWipe_c *thiz);        /* 0x0202fb30 */
extern void *_ZN10FaderColorD2Ev(struct dWipe_c *thiz); /* 0x02017574 */

struct dWipe_c *_ZN7dWipe_cD1Ev(struct dWipe_c *thiz)
{
    thiz->vtable = (void **)data_020926f0;
    if (thiz->needsCleanup == 1)
        func_0202fb30(thiz);
    _ZN10FaderColorD2Ev(thiz);
    return thiz;
}
