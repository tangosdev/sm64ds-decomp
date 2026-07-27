/* Camera::~Camera (D1) at 0x0200705c
 *
 * Inheritance chain (from resolved vtable literals in this function's pool):
 *   Camera : View : ActorDerived : ActorBase
 *     0x02086f84 = _ZTV6Camera
 *     0x02092720 = _ZTV4View
 *     0x0208e4b8 = _ZTV12ActorDerived
 *   bl 0x02043d48 = _ZN9ActorBaseD2Ev (ActorBase::~ActorBase)
 *
 * CodeWarrior virtual-destructor codegen: as the dtor runs, the vptr at
 * [this+0] is reset to each base subobject's vtable in turn, then the
 * immediate base destructor is invoked, and the dtor returns `this`.
 */

/* Minimal layout: only the vtable pointer at offset 0 matters here. */
struct Camera {
    void **vtable;
};

/* Vtable symbols (relocations are wildcarded by the matcher). */
extern void *_ZTV6Camera[];
extern void *_ZTV4View[];
extern void *_ZTV12ActorDerived[];

/* Immediate base destructor. */
extern void _ZN9ActorBaseD2Ev(struct Camera *thiz);

struct Camera *_ZN6CameraD1Ev(struct Camera *thiz)
{
    thiz->vtable = (void **)_ZTV6Camera;
    thiz->vtable = (void **)_ZTV4View;
    thiz->vtable = (void **)_ZTV12ActorDerived;
    _ZN9ActorBaseD2Ev(thiz);
    return thiz;
}
