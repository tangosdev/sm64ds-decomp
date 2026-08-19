// @symbol _ZN7dWipe_cD0Ev
/* dWipe_c::~dWipe_c() (deleting destructor / D0) at 0x0202fbc8.
 *
 *   [this+0] = vtable @0x020926f0 (unnamed, like the rest of this family)
 *   if ((u8)this->byte_f == 1) func_0202fb30(this);
 *   bl 0x02017574 = FaderColor::~FaderColor(this)   (D2, base subobject dtor)
 *   bl 0x0203cbcc = Memory::operator_delete2(this)
 *   return this;
 *
 * Spelled as a plain C function carrying the literal mangled name -- see
 * src/_ZN7dWipe_cD1Ev.c and include/dWipe_c.h for the family convention and
 * full derivation.
 */

struct Obj {
    void **vtable;          /* 0x00 */
    char pad[0xf - 4];
    unsigned char byte_f;   /* 0x0f */
};

extern void *data_020926f0[];                       /* 0x020926f0 */
extern void func_0202fb30(struct Obj *thiz);          /* 0x0202fb30 */
extern void _ZN10FaderColorD2Ev(struct Obj *thiz);          /* 0x02017574 */
extern void _ZN6Memory16operator_delete2EPv(void *p); /* 0x0203cbcc */

struct Obj *_ZN7dWipe_cD0Ev(struct Obj *thiz)
{
    thiz->vtable = (void **)data_020926f0;
    if (thiz->byte_f == 1)
        func_0202fb30(thiz);
    _ZN10FaderColorD2Ev(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
