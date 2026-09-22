/* THE ModelBase AND Model HALF OF port/unmatched/ModelFamily_Dtors_HostCopy.c,
 * for the smoke targets that host those two classes and no other member of the
 * model family.
 *
 * Lane SMOKELINK3, run link100 wave 10 round 2.
 *
 * READ THE PARENT FILE'S HEADER FIRST.  It carries the whole ruling: why the
 * fourteen bodies are host copies at all, what the quarantine is, what it
 * costs, and when it retires.  Nothing here changes any of it.
 *
 * WHY A SUBSET.  The parent covers SIX classes, and four of them --
 * CommonModel, ModelAnim2, BlendModelAnim, ShadowModel -- reach
 * _ZN9AnimationD1Ev, _ZN9ModelAnimD2Ev, data_0209cef4, three _ZTV tables and
 * two Animation thunks that a model smoke does not host.  Taking the parent
 * whole on smoke_model traded four unresolved names for eleven.  This is the
 * same subset argument port/faces_sync_smoke_heap.txt makes for the ledger and
 * port/hal/smoke_aliases_heap.cpp makes for the alias rows: take the rows the
 * target needs, verbatim, and leave the rest where it is.
 *
 * EVERY LINE BELOW IS COPIED CHARACTER FOR CHARACTER from the parent -- the
 * two structs it declares for these classes, the externs these bodies call,
 * and the six bodies themselves (D2, D1 and D0 of each class; D2 is here
 * because both classes' other two bodies call ModelBase's).  Nothing is
 * re-derived, nothing is stubbed, and no target carries both files, so the two
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

extern void *data_0208e87c[];                 /* ModelBase's own table */
extern void *_ZTV5Model[];

extern void Deallocate(void *ptr);                      /* 0x02018144 */
extern void func_0203cbc0(void *ptr);                   /* operator delete */
extern void _ZdlPv(void *ptr);                          /* the same entry */
extern void _ZN6Memory16operator_delete2EPv(void *p);   /* 0x0203cbcc */

extern void _ZN9ModelAnimD2Ev(void *thiz);              /* 0x0201689c */

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
