/* dFdDummy_c::~dFdDummy_c (base-object destructor) at 0x02017254
 * Single-vtable destructor: write own vtable, call base/helper destructor (0x02017574), return this.
 */
struct Obj { void *vtable; };
extern void *data_0208ea6c[];
extern void _ZN10FaderColorD2Ev(struct Obj *thiz); /* 0x02017574 */
struct Obj *_ZN10dFdDummy_cD1Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)data_0208ea6c;
    _ZN10FaderColorD2Ev(thiz);
    return thiz;
}
