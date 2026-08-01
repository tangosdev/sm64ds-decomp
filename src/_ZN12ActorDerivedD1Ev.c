/* _ZN12ActorDerivedD1Ev at 0x02013e80
 * Single-vtable D1/D2 destructor (CodeWarrior 1.2):
 *   write own vtable to [this], call immediate-base destructor, return this.
 * Base dtor call target: 0x02043d48
 */
struct Obj { void *vtable; };
extern void *data_0208e4b8[];
extern void _ZN9ActorBaseD2Ev(struct Obj *thiz); /* 0x02043d48 */
struct Obj *_ZN12ActorDerivedD1Ev(struct Obj *thiz)
{
    thiz->vtable = (void *)data_0208e4b8;
    _ZN9ActorBaseD2Ev(thiz);
    return thiz;
}
