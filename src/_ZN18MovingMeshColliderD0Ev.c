/* MovingMeshCollider::~MovingMeshCollider (deleting / D0) at 0x0203a444
 *   [this+0] = _ZTV18MovingMeshCollider (0x02099434)
 *   bl 0x020397fc = MovingMeshCollider::~MovingMeshCollider (D2/base)
 *   bl 0x0203cbcc = Memory::operator_delete2(this)
 * returns this.
 */

struct MovingMeshCollider {
    void **vtable; /* 0x0 */
};

extern void *_ZTV18MovingMeshCollider[];

extern void *func_020397fc(struct MovingMeshCollider *thiz);  /* 0x020397fc */
extern void _ZN6Memory16operator_delete2EPv(void *ptr);              /* 0x0203cbcc */

struct MovingMeshCollider *_ZN18MovingMeshColliderD0Ev(struct MovingMeshCollider *thiz)
{
    thiz->vtable = (void **)_ZTV18MovingMeshCollider;
    func_020397fc(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
