/* dBgPi's deleting destructor (D0).
 *   [this+0] = vtable (0x02099368)
 *   dBgPc::~dBgPc(this + 4)
 *   Memory::operator_delete2(this) (0x0203cbcc)
 * returns this.
 */

struct Obj {
    void **vtable; /* 0x0 */
};

extern void *data_02099368[];

extern void _ZN5dBgPcD2Ev(void *sub);
extern void _ZN6Memory16operator_delete2EPv(void *ptr);  /* 0x0203cbcc */

struct Obj *_ZN5dBgPiD0Ev(struct Obj *thiz)
{
    thiz->vtable = (void **)data_02099368;
    _ZN5dBgPcD2Ev((char *)thiz + 4);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
