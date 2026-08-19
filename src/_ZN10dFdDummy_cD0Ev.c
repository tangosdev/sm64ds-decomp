/* dFdDummy_c::~dFdDummy_c (deleting destructor) at 0x02017228
 *
 *   [this+0] = vtable @0x0208ea6c (unnamed in verified.tsv)
 *   bl 0x02017574 = Color::~Color(this)   (immediate base dtor)
 *   bl 0x0203cbcc = Memory::operator_delete2(this)
 *   return this;
 */

struct Obj {
    void **vtable;   /* 0x00 */
};

extern void *data_0208ea6c[];                       /* 0x0208ea6c */
extern void _ZN10FaderColorD2Ev(struct Obj *thiz);          /* 0x02017574 */
extern void _ZN6Memory16operator_delete2EPv(void *p); /* 0x0203cbcc */

struct Obj *_ZN10dFdDummy_cD0Ev(struct Obj *thiz)
{
    thiz->vtable = (void **)data_0208ea6c;
    _ZN10FaderColorD2Ev(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
