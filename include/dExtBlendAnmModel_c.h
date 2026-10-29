#ifndef DEXTBLENDANMMODEL_C_H
#define DEXTBLENDANMMODEL_C_H

#include "types.h"
#include "dExtAnmModel_c.h"

/* dExtAnmModel_c child that cross-fades between the previous pose and the new
 * animation, vtable _ZTV19dExtBlendAnmModel_c at 0x0208e94c (same seven-slot
 * shape as dExtAnmModel_c, every slot overridden) plus its own secondary
 * dExtFrameCtrl_c table at _ZTV19dExtBlendAnmModel_c + 0x24
 * (VTable_dExtFrameCtrl_c_dExtBlendAnmModel_cThunk).
 *
 * THE DESTRUCTOR IS DECLARED FIRST -- see include/dExtAnmModel_c.h; the D1
 * additionally frees mBlendBuffer before chaining to dExtAnmModel_c::D2. Defined
 * out of line in src/engine/model/d_ext_model.cpp. That definition is
 * the key function, so the TU also emits the MI vtable group with its RTTI,
 * and the retail table stays externalized (see the TU manifest).
 *
 * LAYOUT: dExtAnmModel_c 0x00..0x64, then the blend state. blendWeight ramps
 * 0 -> 0x1000 (1.0) by blendStep per Advance; below 1.0 UpdateVerts and
 * Virtual10 route through the blending func_0204531c instead of
 * UpdateVertsUsingBones. mBlendBuffer is a buffer the destructor frees and
 * func_020165c4 (DoSetFile's tail) sets up.
 *
 * SetAnim IS A REAL METHOD, and its symbol is
 * _ZN19dExtBlendAnmModel_c7SetAnimER8BCA_Fileiiit. It used to be a mangled
 * extern "C" free function taking an explicit `thiz`, on the grounds that wall
 * 6az forbade the method form. That was wrong: the wall was the NAME. The old
 * symbol spelled a by-value `Fix12<int>` that we invented -- the cartridge
 * carries no mangled names at all -- and under the honest scalar `Fix12i` the
 * method reproduces 0x020163e0/0x9c exactly. See notes/mwccarm-codegen.md
 * section 6az, MEASURED 2026-10-22.
 */

#ifdef __cplusplus

namespace Memory {
void operator_delete2(void *);
}

struct dExtBlendAnmModel_c : dExtAnmModel_c {
    Fix12i blendWeight;        /* 0x64 - 0x1000 is 1.0 */
    Fix12i blendStep;          /* 0x68 - per-frame increment */
    void *mBlendBuffer; /* 0x6c - pose blend buffer; DoSetFile tail allocates it, D1 frees it */

    /* --- vtable order. Do not reorder. --- */
    /* The destructor pair spelled as two plain virtuals on the host, plus the
       non-virtual destructor declaration the src/ definitions need; the whole
       ruling is in include/dExtModel_c.h. Overrides take their base's slots, so
       these carry the SAME TWO NAMES dExtAnmModel_c declares. */
#ifdef _MSC_VER
    virtual void Destructor1();                           /* slot 0 (D1) */
    virtual void Destructor0();                           /* slot 1 (D0) */
    ~dExtBlendAnmModel_c();                               /* no slot */
#else
    virtual ~dExtBlendAnmModel_c(); /* slots 0 (D1), 1 (D0) */
#endif
    virtual int DoSetFile(char *file, int a, int b);      /* slot 2 */
    virtual void UpdateVerts();                           /* slot 3 */
    virtual void Virtual10(Matrix4x3 &mat);               /* slot 4 */
    virtual void Render(const Vector3 *scale);            /* slot 5 */
    virtual void Virtual18(u32 mat, const Vector3 *scale);/* slot 6 */

    /* DECLARED, defined out of line in src/engine/model/d_ext_model.cpp
     * as real C++ -- complete-object context for every ROM caller (it is a
     * member subobject in daKing_Donketu_c/Eyerok/daJango_c/daMoray_c...), hence C1.
     * Body is empty; blendWeight and mBlendBuffer are initialised through the init
     * list so their stores land where the ROM has them. */

    /* --- non-virtual --- */
    dExtBlendAnmModel_c();
    void Advance();
    void SetAnim(BCA_File &animFile, int numBlendFrames, int flags, Fix12i speed, u16 startFrame);

    /* ITS OWN, TO RESOLVE AN AMBIGUITY MULTIPLE INHERITANCE CREATES. dExtAnmModel_c
       derives from dExtSimpleModel_c (so dExtModel_c) and from dExtFrameCtrl_c, and both bases
       declare operator delete, so an inherited one is "ambiguous access to
       name found: dExtModel_c::operator delete and dExtFrameCtrl_c::operator delete".
       Declaring it here picks the same deallocator both bases name, and also
       satisfies the rule in include/dActor_c.h that mwcc only inlines the member
       when it is in the class or its immediate base. */
    void operator delete(void *ptr) {
        Memory::operator_delete2(ptr);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dExtBlendAnmModel_c_size_must_be_0x70[sizeof(dExtBlendAnmModel_c) == 0x70 ? 1 : -1];
#endif

#else

/* The same object for C translation units, both vptrs written out -- the same shape
 * dExtAnmModel_c.h gives its own C fallback, with the blend state appended. */
struct dExtBlendAnmModel_c {
    void **vtable;                     /* 0x00 */
    struct BMD_File *modelFile;        /* 0x04 */
    struct ModelComponents data;       /* 0x08 */
    struct Matrix4x3 mat4x3;           /* 0x1c */
    void *transformsBuf;               /* 0x4c */
    void **animVtable;                 /* 0x50 */
    u32 numFramesAndFlags;             /* 0x54 */
    s32 currFrame;                     /* 0x58 */
    s32 speed;                         /* 0x5c */
    struct BCA_File *file;             /* 0x60 */
    s32 blendWeight;                   /* 0x64 */
    s32 blendStep;                     /* 0x68 */
    void *mBlendBuffer; /* 0x6c - pose blend buffer; DoSetFile tail allocates it, D1 frees it */
};

/* So an object header declaring a dExtBlendAnmModel_c member reads the same in both modes:
 * C++ gets the class, C gets the flat stand-in above, and neither needs `struct`. */
typedef struct dExtBlendAnmModel_c dExtBlendAnmModel_c;

#endif /* __cplusplus */

#endif /* DEXTBLENDANMMODEL_C_H */
