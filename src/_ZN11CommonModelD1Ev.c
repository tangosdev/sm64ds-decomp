/* CommonModel::~CommonModel (D1) at 0x020161e0
 * Single-vtable destructor: write own vtable, call ModelBase::~ModelBase (D2 @0x020170b8), return this.
 */
struct Obj { void *vtable; };
extern void *_ZTV11CommonModel[];
extern void _ZN9ModelBaseD2Ev(struct Obj *thiz); /* 0x020170b8 */
struct Obj *_ZN11CommonModelD1Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)_ZTV11CommonModel;
    _ZN9ModelBaseD2Ev(thiz);
    return thiz;
}
