/* func_0202fbc8 (deleting destructor) at 0x0202fbc8
 *
 *   [this+0] = vtable @0x020926f0 (unnamed in verified.tsv)
 *   if ((u8)this->byte_f == 1) func_0202fb30(this);
 *   bl 0x02017574 = Color::~Color(this)   (immediate base dtor)
 *   bl 0x0203cbcc = Memory::operator_delete2(this)
 *   return this;
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

struct Obj *func_0202fbc8(struct Obj *thiz)
{
    thiz->vtable = (void **)data_020926f0;
    if (thiz->byte_f == 1)
        func_0202fb30(thiz);
    _ZN10FaderColorD2Ev(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
