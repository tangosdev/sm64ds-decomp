// @symbol _ZN7dWipe_cD1Ev
/* dWipe_c::~dWipe_c() (complete-object / D1) at 0x0202fc08.
 *   [this+0] = vtable (0x020926f0)
 *   if (this->unk_00f == 1) func_0202fb30(this);   (bl 0x0202fb30)
 *   bl 0x02017574 = FaderColor::~FaderColor (D2, base subobject dtor)
 * returns this.
 *
 * Spelled as a plain C function carrying the literal mangled name, matching
 * sibling convention (src/_ZN10FaderColorD2Ev.c,
 * src/engine/fader/_ZN10FaderColorD1Ev.c): no TU defines dWipe_c's D1 as a
 * real C++ member function, so it never collides with the ROM's vtable
 * group. See include/dWipe_c.h for full derivation.
 */

struct Obj {
    void **vtable;   /* 0x0 */
    char pad[0xf - 4];
    unsigned char unkF; /* 0xf */
};

extern void *data_020926f0[];

extern void func_0202fb30(struct Obj *thiz);    /* 0x0202fb30 */
extern void *_ZN10FaderColorD2Ev(struct Obj *thiz);   /* 0x02017574 */

struct Obj *_ZN7dWipe_cD1Ev(struct Obj *thiz)
{
    thiz->vtable = (void **)data_020926f0;
    if (thiz->unkF == 1)
        func_0202fb30(thiz);
    _ZN10FaderColorD2Ev(thiz);
    return thiz;
}
