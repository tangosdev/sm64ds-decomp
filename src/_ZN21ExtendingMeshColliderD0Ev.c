/* ExtendingMeshCollider::~ExtendingMeshCollider (deleting / D0) at 0x0203ab3c
 *   [this+0] = _ZTV21ExtendingMeshCollider (0x02099490)
 *   bl 0x0203a420 = ExtendingMeshCollider::~ExtendingMeshCollider (D2/base)
 *   bl 0x0203cbcc = Memory::operator_delete2(this)
 * returns this.
 */

struct ExtendingMeshCollider {
    void **vtable; /* 0x0 */
};

extern void *_ZTV21ExtendingMeshCollider[];

extern void *_ZN18MovingMeshColliderD2Ev(struct ExtendingMeshCollider *thiz); /* 0x0203a420 */
extern void _ZN6Memory16operator_delete2EPv(void *ptr);                   /* 0x0203cbcc */

struct ExtendingMeshCollider *_ZN21ExtendingMeshColliderD0Ev(struct ExtendingMeshCollider *thiz)
{
    thiz->vtable = (void **)_ZTV21ExtendingMeshCollider;
    _ZN18MovingMeshColliderD2Ev(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
