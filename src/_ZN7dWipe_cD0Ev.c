// @symbol _ZN7dWipe_cD0Ev
/* dWipe_c::~dWipe_c() -- deleting destructor (D0), vtable slot 1, at
 * 0x0202fbc8. Cancels the capture if one is still armed, runs the FaderColor
 * base destructor, then frees.
 *
 * Plain C carrying the literal mangled name, matching the family convention:
 * no TU defines dWipe_c's destructor as a real C++ member, so nothing collides
 * with the ROM's own vtable group. See include/dWipe_c.h and
 * notes/scene-provenance.md.
 */

struct dWipe_c {
    void **vtable;              /* 0x00 */
    char pad[0xf - 4];
    unsigned char needsCleanup; /* 0x0f */
};

extern void *data_020926f0[];                          /* dWipe_c's vtable */
extern void func_0202fb30(struct dWipe_c *thiz);       /* 0x0202fb30 */
extern void _ZN10FaderColorD2Ev(struct dWipe_c *thiz); /* 0x02017574 */
extern void _ZN6Memory16operator_delete2EPv(void *p);  /* 0x0203cbcc */

struct dWipe_c *_ZN7dWipe_cD0Ev(struct dWipe_c *thiz)
{
    thiz->vtable = (void **)data_020926f0;
    if (thiz->needsCleanup == 1)
        func_0202fb30(thiz);
    _ZN10FaderColorD2Ev(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
