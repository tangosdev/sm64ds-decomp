/* Camera::~Camera (D1) at 0x0200705c
 *
 * Inheritance chain (from resolved vtable literals in this function's pool):
 *   Camera : View : dBase_c : fBase_c
 *     0x02086f84 = _ZTV6Camera
 *     0x02092720 = _ZTV4View
 *     0x0208e4b8 = data_0208e4b8
 *   bl 0x02043d48 = _ZN7fBase_cD2Ev (fBase_c::~fBase_c)
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
extern void *data_0208e4b8[];

/* Immediate base destructor. */
extern void _ZN7fBase_cD2Ev(struct Camera *thiz);

struct Camera *_ZN6CameraD1Ev(struct Camera *thiz)
{
    thiz->vtable = (void **)_ZTV6Camera;
    thiz->vtable = (void **)_ZTV4View;
    thiz->vtable = (void **)data_0208e4b8;
    _ZN7fBase_cD2Ev(thiz);
    return thiz;
}
