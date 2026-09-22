/* THE MODEL FAMILY'S FOURTEEN DESTRUCTOR BODIES, HOST-COPIED.
 *
 * Written at the main -> port sync (run link100 wave 7, lane SYNC5) under
 * Tango's quarantine ruling, and it is the second road of
 * runs/link100/out/SYNC4/decomp_side.md item 6. Read that item before changing
 * anything here.
 *
 * WHAT HAPPENED. Five host headers -- include/ModelBase.h:127 and the same
 * block in Model.h, ModelAnim2.h, CommonModel.h, BlendModelAnim.h and
 * ShadowModel.h -- spell the destructor as TWO PLAIN VIRTUALS under _MSC_VER,
 * "virtual void Destructor1()" for ROM slot 0 and "virtual void Destructor0()"
 * for slot 1, where the DS arm has one "virtual ~ModelBase()". That is
 * deliberate: mwccarm gives one virtual destructor the ROM's TWO vtable slots
 * and MSVC folds them into one, which would shift every virtual below the
 * destructor by a slot for the whole hierarchy.
 *
 * Ten of these thirteen TUs were .c files at this branch's merge base
 * (port/l4-merge 0046a2d11): as C they compiled against the header's #else
 * arm -- a plain struct with a vtable word -- and emitted the FLAT symbol with
 * this on the stack, which is exactly what port/hal/model_dtor_seat.cpp's
 * adapters and a dozen actor-class tables call. main has since converted them
 * to .cpp, where each defines X::~X(): a destructor the host header does not
 * declare, so MSVC refuses the TU with C2600 and no host option, pragma or
 * per-source definition reaches it. Seventeen build objects over thirteen
 * sources, and with them the whole link. ShadowModel::~ShadowModel D0 joined
 * them one build later: lane SEAT2 had seated it from src, and main has the
 * same .cpp shape there.
 *
 * WHAT THIS FILE IS. The fourteen bodies exactly as this branch's merge base
 * carried them (git show 0046a2d11:src/<name>.c), with the per-file structs
 * merged into one set of shapes so they can share a translation unit. Nothing
 * is re-derived and nothing is guessed: every body, every field offset and
 * every call is the text the port compiled and shipped before the merge, and
 * each one is byte-verified against the ROM in its own src header there.  The
 * fourteen src rows are quarantined in their slices with a pointer back here.
 *
 * WHAT IT COSTS AND WHEN IT GOES. Fourteen translation units off the linkage
 * count, which this branch was carrying before the merge and would otherwise
 * lose outright along with the link. The file retires the moment the decomp
 * answers item 6 -- give the class a host-visible destructor spelling, or the
 * D0/D1/D2 bodies a declaration the host header carries -- at which point the
 * quarantined rows come back and this file is deleted in the same commit.
 *
 * C, NOT C++, ON PURPOSE: these are the flat Itanium-mangled names the ROM's
 * own tables hold, and a .c TU emits them with no decoration of its own.
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

struct MdlAnim2 {
    void *vtable;            /* 0x00 */
    char pad0[0x50 - 4];
    void *animVtable;        /* 0x50 Animation sub-object */
};

struct CommonMdl {
    void *vtable;            /* 0x00 */
};

struct BlendMdlAnim {
    void *vtable;            /* 0x00 */
    char pad0[0x50 - 4];
    void *animVtable;        /* 0x50 Animation sub-object */
    char pad1[0x6c - 0x54];
    void *unk6c;             /* 0x6c owned pointer */
};

struct ShadowMdl {
    void *vtable;            /* 0x00 */
    char pad[0x20 - 4];
    struct ShadowMdl *prev;  /* 0x20 */
    struct ShadowMdl *next;  /* 0x24 */
};

extern void *data_0208e87c[];                 /* ModelBase's own table */
extern void *_ZTV5Model[];
extern void *_ZTV10ModelAnim2[];
extern void *_ZTV11CommonModel[];
extern void *_ZTV14BlendModelAnim[];
extern void *_ZTV11ShadowModel[];
extern void *VTable_Animation_ModelAnim2Thunk[];
extern void *VTable_Animation_BlendModelAnimThunk[];
extern struct ShadowMdl *data_0209cef4;       /* the shadow list head */

extern void Deallocate(void *ptr);                      /* 0x02018144 */
extern void func_0203cbc0(void *ptr);                   /* operator delete */
extern void _ZdlPv(void *ptr);                          /* the same entry */
extern void _ZN6Memory16operator_delete2EPv(void *p);   /* 0x0203cbcc */
extern void _ZN9AnimationD1Ev(void *thiz);
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

/* ---- ModelAnim2 (D1, D0) ------------------------------------------------ */

struct MdlAnim2 *_ZN10ModelAnim2D1Ev(struct MdlAnim2 *thiz)
{
    thiz->vtable = (void *)_ZTV10ModelAnim2;
    thiz->animVtable = (void *)VTable_Animation_ModelAnim2Thunk;
    _ZN9AnimationD1Ev((char *)thiz + 0x68);
    _ZN9ModelAnimD2Ev(thiz);
    return thiz;
}

struct MdlAnim2 *_ZN10ModelAnim2D0Ev(struct MdlAnim2 *thiz)
{
    thiz->vtable = (void *)_ZTV10ModelAnim2;
    thiz->animVtable = (void *)VTable_Animation_ModelAnim2Thunk;
    _ZN9AnimationD1Ev((char *)thiz + 0x68);
    _ZN9ModelAnimD2Ev(thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}

/* ---- CommonModel, 0x020161e0 (D1) --------------------------------------- */

struct CommonMdl *_ZN11CommonModelD1Ev(struct CommonMdl *thiz)
{
    thiz->vtable = (void *)_ZTV11CommonModel;
    _ZN9ModelBaseD2Ev((struct MdlBase *)thiz);
    return thiz;
}

struct CommonMdl *_ZN11CommonModelD0Ev(struct CommonMdl *thiz)
{
    thiz->vtable = (void *)_ZTV11CommonModel;
    _ZN9ModelBaseD2Ev((struct MdlBase *)thiz);
    _ZN6Memory16operator_delete2EPv(thiz);
    return thiz;
}

/* ---- BlendModelAnim, 0x02016690 (D1), 0x02016644 (D0) ------------------- */

struct BlendMdlAnim *_ZN14BlendModelAnimD1Ev(struct BlendMdlAnim *thiz)
{
    thiz->vtable = (void *)_ZTV14BlendModelAnim;
    thiz->animVtable = (void *)VTable_Animation_BlendModelAnimThunk;
    if (thiz->unk6c)
        _ZdlPv(thiz->unk6c);
    _ZN9ModelAnimD2Ev(thiz);
    return thiz;
}

struct BlendMdlAnim *_ZN14BlendModelAnimD0Ev(struct BlendMdlAnim *thiz)
{
    thiz->vtable = (void *)_ZTV14BlendModelAnim;
    thiz->animVtable = (void *)VTable_Animation_BlendModelAnimThunk;
    if (thiz->unk6c)
        func_0203cbc0(thiz->unk6c);
    _ZN9ModelAnimD2Ev(thiz);
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
