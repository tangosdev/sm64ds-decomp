/* dBgCh_Actr::~dBgCh_Actr (D1/complete) at 0x020373f8
 *
 *   [this+0] = vtable @0x02099204 (unnamed in verified.tsv)
 *   bl 0x02037764 = dBgCh_Lin::~dBgCh_Lin(this+0x134)  (member subobject)
 *   bl 0x02037cb0 = dBgCh_SphCrr::~dBgCh_SphCrr(this+0x20)      (member subobject)
 *   bl 0x020354d0 = <base>::~<base>(this)                   (immediate base dtor)
 *   return this;
 */

struct dBgCh_Actr {
    void **vtable;            /* 0x000 */
    char pad0[0x20 - 4];
    void **sphereVtable;      /* 0x020: dBgCh_SphCrr member subobject */
    char pad1[0x134 - 0x24];
    void **raycastVtable;     /* 0x134: dBgCh_Lin member subobject */
};

extern void *_ZTV10dBgCh_Actr[];                          /* 0x02099204 */
extern void _ZN9dBgCh_LinD1Ev(void *raycast);        /* 0x02037764 */
extern void _ZN12dBgCh_SphCrrD1Ev(void *sphere);          /* 0x02037cb0 */
extern void _ZN5dBgChD2Ev(struct dBgCh_Actr *self); /* 0x020354d0 (base dtor) */

struct dBgCh_Actr *_ZN10dBgCh_ActrD1Ev(struct dBgCh_Actr *self)
{
    self->vtable = (void **)_ZTV10dBgCh_Actr;
    _ZN9dBgCh_LinD1Ev((char *)self + 0x134);
    _ZN12dBgCh_SphCrrD1Ev((char *)self + 0x20);
    _ZN5dBgChD2Ev(self);
    return self;
}
