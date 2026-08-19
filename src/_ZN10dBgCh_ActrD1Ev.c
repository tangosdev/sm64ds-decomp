/* WithMeshClsn::~WithMeshClsn (D1/complete) at 0x020373f8
 *
 *   [this+0] = vtable @0x02099204 (unnamed in verified.tsv)
 *   bl 0x02037764 = RaycastLine::~RaycastLine(this+0x134)  (member subobject)
 *   bl 0x02037cb0 = SphereClsn::~SphereClsn(this+0x20)      (member subobject)
 *   bl 0x020354d0 = <base>::~<base>(this)                   (immediate base dtor)
 *   return this;
 */

struct WithMeshClsn {
    void **vtable;            /* 0x000 */
    char pad0[0x20 - 4];
    void **sphereVtable;      /* 0x020: SphereClsn member subobject */
    char pad1[0x134 - 0x24];
    void **raycastVtable;     /* 0x134: RaycastLine member subobject */
};

extern void *data_02099204[];                          /* 0x02099204 */
extern void _ZN11RaycastLineD1Ev(void *raycast);        /* 0x02037764 */
extern void _ZN10SphereClsnD1Ev(void *sphere);          /* 0x02037cb0 */
extern void func_020354d0(struct WithMeshClsn *self); /* 0x020354d0 (base dtor) */

struct WithMeshClsn *_ZN12WithMeshClsnD1Ev(struct WithMeshClsn *self)
{
    self->vtable = (void **)data_02099204;
    _ZN11RaycastLineD1Ev((char *)self + 0x134);
    _ZN10SphereClsnD1Ev((char *)self + 0x20);
    func_020354d0(self);
    return self;
}
