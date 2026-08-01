struct WithMeshClsn { void **vtable; };
extern void *data_02099204[];
extern void _ZN11RaycastLineD1Ev(void *thiz);
extern void _ZN10SphereClsnD1Ev(void *thiz);
extern void func_020354d0(struct WithMeshClsn *thiz);   /* base D2 @0x020354d0 */
extern void _ZN6Memory16operator_delete2EPv(void *ptr);

struct WithMeshClsn *func_020373b8(struct WithMeshClsn *thiz)
{
    thiz->vtable = (void **)data_02099204;
    _ZN11RaycastLineD1Ev((char *)thiz + 0x134);
    _ZN10SphereClsnD1Ev((char *)thiz + 0x20);
    func_020354d0(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
