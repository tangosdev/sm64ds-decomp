/* THE ModelBase, Model AND ShadowModel PART OF port/unmatched/ModelFamily_Dtors_HostCopy.c,
 * for the smoke targets that host those three classes and no other member of
 * the model family.
 *
 * Lane SMOKELINK5, run link100 wave 10 round 5, applying out/SMOKELINK4B/
 * remaining.md section A verbatim.
 *
 * READ THE PARENT FILE'S HEADER FIRST.  It carries the whole ruling: why the
 * fourteen bodies are host copies at all, what the quarantine is, what it
 * costs, and when it retires.  Nothing here changes any of it.
 *
 * WHY A SUBSET.  The parent covers SIX classes, and the three this target does
 * not host -- CommonModel, ModelAnim2, BlendModelAnim -- reach
 * _ZN9AnimationD1Ev, _ZTV10ModelAnim2, _ZTV14BlendModelAnim,
 * VTable_Animation_ModelAnim2Thunk and VTable_Animation_BlendModelAnimThunk.
 * This is the same subset argument port/faces_sync_smoke_heap.txt makes for
 * the ledger and port/hal/smoke_aliases_heap.cpp makes for the alias rows:
 * take the rows the target needs, verbatim, and leave the rest where it is.
 * port/unmatched/ModelBaseModel_Dtors_HostCopy.c is exactly this shape for
 * ModelBase and Model alone (smoke_model's two-class gate); this file is that
 * same text plus ShadowModel's D1 and D0, which this batch's shadow gate
 * needs and smoke_model's targets do not.
 *
 * EVERY LINE BELOW IS COPIED CHARACTER FOR CHARACTER from the parent -- the
 * three structs it declares for these classes, the externs these bodies call,
 * and the eight bodies themselves (ModelBase's and Model's D2, D1 and D0, and
 * ShadowModel's D1 and D0; ModelBase's D2 is here because ShadowModel's D1 and
 * D0 both call it, the same way Model's does).  Nothing is re-derived, nothing
 * is stubbed, and no target carries both this file and either
 * ModelBaseModel_Dtors_HostCopy.c or ModelFamily_Dtors_HostCopy.c, so the
 * copies cannot collide.  If the parent's text for one of these ever changes,
 * the two files disagree and the diff is one grep away.
 *
 * C, NOT C++, ON PURPOSE, for the parent's reason: these are the flat
 * Itanium-mangled names the ROM's own tables hold, and a .c TU emits them with
 * no decoration of its own.
 */

struct MdlBase {
    void *vtable;            /* 0x00 */
    void *res;               /* 0x04 owned resource */
};

struct Mdl {
    void *vtable;            /* 0x00 */
    char pad[0x4c - 4];
    void *unk4c;             /* 0x4c owned pointer */
};

struct ShadowMdl {
    void *vtable;            /* 0x00 */
    char pad[0x20 - 4];
    struct ShadowMdl *prev;  /* 0x20 */
    struct ShadowMdl *next;  /* 0x24 */
};

extern void *data_0208e87c[];                 /* ModelBase's own table */
extern void *_ZTV5Model[];
extern void *_ZTV11ShadowModel[];
extern struct ShadowMdl *data_0209cef4;       /* the shadow list head */

extern void Deallocate(void *ptr);                      /* 0x02018144 */
extern void func_0203cbc0(void *ptr);                   /* operator delete */
extern void _ZdlPv(void *ptr);                          /* the same entry */
extern void _ZN6Memory16operator_delete2EPv(void *p);   /* 0x0203cbcc */

/* ---- ModelBase, 0x020170b8 (D2), 0x02017120 (D1), 0x020170e8 (D0) -------- */

struct MdlBase *_ZN9ModelBaseD2Ev(struct MdlBase *thiz)
{
    thiz->vtable = (void *)data_0208e87c;
    if (thiz->res != 0) {
        Deallocate(thiz->res);
    }
    return thiz;
}

struct MdlBase *_ZN9ModelBaseD1Ev(struct MdlBase *thiz)
{
    thiz->vtable = (void *)data_0208e87c;
    if (thiz->res != 0) {
        Deallocate(thiz->res);
    }
    return thiz;
}

struct MdlBase *_ZN9ModelBaseD0Ev(struct MdlBase *thiz)
{
    thiz->vtable = (void *)data_0208e87c;
    if (thiz->res)
        Deallocate(thiz->res);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}


/* ---- Model, 0x02016ca8 (D2), 0x02016d20 (D1), 0x02016ce0 (D0) ------------ */

struct Mdl *_ZN5ModelD2Ev(struct Mdl *thiz)
{
    thiz->vtable = (void *)_ZTV5Model;
    if (thiz->unk4c)
        _ZdlPv(thiz->unk4c);
    _ZN9ModelBaseD2Ev((struct MdlBase *)thiz);
    return thiz;
}

struct Mdl *_ZN5ModelD1Ev(struct Mdl *thiz)
{
    thiz->vtable = (void *)_ZTV5Model;
    if (thiz->unk4c)
        _ZdlPv(thiz->unk4c);
    _ZN9ModelBaseD2Ev((struct MdlBase *)thiz);
    return thiz;
}

struct Mdl *_ZN5ModelD0Ev(struct Mdl *thiz)
{
    thiz->vtable = (void *)_ZTV5Model;
    if (thiz->unk4c)
        func_0203cbc0(thiz->unk4c);
    _ZN9ModelBaseD2Ev((struct MdlBase *)thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}

/* ---- ShadowModel, 0x02015ff8 (D1) --------------------------------------- */

struct ShadowMdl *_ZN11ShadowModelD1Ev(struct ShadowMdl *thiz)
{
    struct ShadowMdl *prev;
    struct ShadowMdl *next;

    thiz->vtable = (void *)_ZTV11ShadowModel;

    prev = thiz->prev;
    if (prev) {
        prev->next = thiz->next;
    } else if (data_0209cef4 == thiz) {
        data_0209cef4 = thiz->next;
    }

    next = thiz->next;
    if (next) {
        next->prev = thiz->prev;
    }

    thiz->prev = 0;
    thiz->next = 0;

    _ZN9ModelBaseD2Ev((struct MdlBase *)thiz);
    return thiz;
}

struct ShadowMdl *_ZN11ShadowModelD0Ev(struct ShadowMdl *thiz)
{
    thiz->vtable = (void *)_ZTV11ShadowModel;

    if (thiz->prev)
        thiz->prev->next = thiz->next;
    else if (data_0209cef4 == thiz)
        data_0209cef4 = thiz->next;

    if (thiz->next)
        thiz->next->prev = thiz->prev;

    thiz->prev = 0;
    thiz->next = 0;

    _ZN9ModelBaseD2Ev((struct MdlBase *)thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}
