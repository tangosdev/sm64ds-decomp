/* func_0203a4b8 at 0x02014878
 * MovingMeshCollider C1 (complete object) constructor:
 *   call base dCc_c::dCc_c() (C2), then set own vtable.
 */
struct Obj { void *vtable; };
extern void *_ZTV18MovingMeshCollider[];
extern void func_020398c8(struct Obj *thiz); /* 0x020150cc */

struct Obj *func_0203a4b8(struct Obj *thiz)
{
    func_020398c8(thiz);
    thiz->vtable = (void *)_ZTV18MovingMeshCollider;
    return thiz;
}
