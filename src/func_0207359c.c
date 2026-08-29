/* func_0207359c at 0x0207359c - a deleting destructor (D0).
 *   [this+0] = vtable (0x0209a754)
 *   bl 0x0207363c = base destructor (D2)
 *   bl 0x0203cbcc = Memory::operator_delete2(this)
 * returns this.
 */

struct Obj { void **vtable; };

extern void *_ZTVN3abi21__vmi_class_type_infoE[];
extern void *func_0207363c(struct Obj *thiz);       /* 0x0207363c */
extern void _ZN6Memory16operator_delete2EPv(void *ptr); /* 0x0203cbcc */

struct Obj *func_0207359c(struct Obj *thiz)
{
    thiz->vtable = (void **)_ZTVN3abi21__vmi_class_type_infoE;
    func_0207363c(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
