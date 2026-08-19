/* func_02038114 at 0x02038114 - a deleting destructor (D0).
 *   [this+0] = vtable (0x02099368)
 *   func_02037ee4(this + 4)        (destroys a subobject at +4)
 *   Memory::operator_delete2(this) (0x0203cbcc)
 * returns this.
 */

struct Obj {
    void **vtable; /* 0x0 */
};

extern void *_ZTV5dBgPi[];

extern void func_02037ee4(void *sub);                    /* 0x02037ee4 */
extern void _ZN6Memory16operator_delete2EPv(void *ptr);  /* 0x0203cbcc */

struct Obj *func_02038114(struct Obj *thiz)
{
    thiz->vtable = (void **)_ZTV5dBgPi;
    func_02037ee4((char *)thiz + 4);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
