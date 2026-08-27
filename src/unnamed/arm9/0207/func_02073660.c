/* func_02073660 at 0x02073660 - a deleting destructor (D0).
 *   [this+0] = vtable (0x0209a774)
 *   bl 0x020736b0 = base destructor (D2)
 *   bl 0x0203cbcc = Memory::operator_delete2(this)
 * returns this.
 */

struct Obj { void **vtable; };

extern void *data_0209a774[];
extern void *func_020736b0(struct Obj *thiz);       /* 0x020736b0 */
extern void _ZN6Memory16operator_delete2EPv(void *ptr); /* 0x0203cbcc */

struct Obj *func_02073660(struct Obj *thiz)
{
    thiz->vtable = (void **)data_0209a774;
    func_020736b0(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
