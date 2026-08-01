/* func_02017254 at 0x02017254
 * Single-vtable destructor: write own vtable, call base/helper destructor (0x02017574), return this.
 */
struct Obj { void *vtable; };
extern void *data_0208ea6c[];
extern void _ZN5ColorD1Ev(struct Obj *thiz); /* 0x02017574 */
struct Obj *func_02017254(struct Obj *thiz)
{
    thiz->vtable = (void *)data_0208ea6c;
    _ZN5ColorD1Ev(thiz);
    return thiz;
}
