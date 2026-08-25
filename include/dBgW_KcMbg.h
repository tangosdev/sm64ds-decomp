#ifndef DBGW_KCMBG_H
#define DBGW_KCMBG_H

#include "types.h"
#include "dBgW_Kc.h"
#include "math/Matrix.h"

/* The moving-mesh collider, vtable _ZTV10dBgW_KcMbg at 0x02099434.
 * Derives from dBgW_Kc (SetFile chains to dBgW_Kc::SetFile and
 * the fields start at exactly 0x50); dBgW_KcMbgSclY derives from
 * this in turn. Overrides every slot except BeforeClsn (slot 9) and
 * GetSurfaceInfo (slot 3), both of which it inherits unchanged.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND D1 IS A REAL METHOD -- see
 * include/ModelBase.h for the key-function rule and the objisolate exemption
 * to it. D0 and D2 stay C files.
 *
 * SetFile and Transform keep extern-C free definitions for now: their
 * bodies are dominated by flat 12-word Matrix4x3 copies (the
 * Model::Virtual10 lesson) and belong to a later pass with a shared
 * flat-copy helper. Their declarations here are the real signatures
 * (SetFile's carries Fix12<int>, wall 6az, so its definition could not
 * be a method yet anyway).
 */

#ifdef __cplusplus

#include "math/Fix12.h"

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct dBgW_KcMbg : dBgW_Kc {
    Fix12i scale;              /* 0x50 */
    Matrix4x3 mat;             /* 0x54 */
    Matrix4x3 invRotMat;       /* 0x84 - rotation-only inverse */
    Matrix4x3 scaledMat;       /* 0xb4 */
    Matrix4x3 invMat;          /* 0xe4 - previous frame's inverse in Transform */
    s16 angY;                  /* 0x114 */
    s16 angVelY;               /* 0x116 */
    Vector3 pos;               /* 0x118 */
    Vector3 velocity;          /* 0x124 */
    u32 unk_130;              /* 0x130 */
    Matrix4x3 newScaledMat;    /* 0x134 */
    s32 invScale;               /* 0x164 - func_02053200(scale) */
    Matrix4x3 invScaledMat;    /* 0x168 */
    Matrix4x3 prevInvScaledMat;/* 0x198 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dBgW_KcMbg();                        /* slots 0/1 */
    virtual void Virtual08();                             /* slot 2 */
    /* slot 3 GetSurfaceInfo is NOT overridden: this table's word is
       0x01ffd920, the same one dBgW_Kc holds, so it is inherited. */
    virtual void GetNormal(s16 triID, Vector3 &res);      /* slot 4 */
    virtual void GetTriangleOrigin(s16 triID, Vector3 &res); /* slot 5 */
    virtual int DetectClsn(dBgCh_Gnd &ray);           /* slot 6 - free def */
    virtual int DetectClsn(dBgCh_Lin &ray);             /* slot 7 - free def */
    virtual int DetectClsn(dBgCh_SphCrr &sphere);           /* slot 8 - free def */
    virtual int TransformPos(const Vector3 &pos, Vector3 &res); /* slot 10 */
    virtual s16 GetAngularVelY();                         /* slot 11 */
    virtual void GetVelocity(Vector3 &res);               /* slot 12 */

    /* DECLARED, never defined as a method here -- src/_ZN10dBgW_KcMbgC1Ev.cpp
       owns C1 and src/_ZN10dBgW_KcMbgC2Ev.c the base-subobject variant
       (notes/ctor-migration.md section 2). */
    dBgW_KcMbg();

    /* --- non-virtual --- */
    void SetFile(KCL_File *file, const Matrix4x3 &mat, Fix12<int> scale,
                 s16 angY, CLPS_Block &clps);             /* free def, wall 6az */
    void Transform(const Matrix4x3 &mat, s16 angY);       /* free def for now */

    /* WHAT LETS A REAL `~Class()` REPRODUCE THE ROM'S DELETING DESTRUCTOR.
       The compiler generates D0 as "run the destructor body, then call operator
       delete on the class". Without this it emits the global `_ZdlPv`, which
       exists nowhere in this image, and the D0 comes out one relocated word
       different from the ROM -- a difference build_pin.verify CANNOT SEE,
       because it wildcards relocated words. Only the link catches it.

       This family deallocates through Memory::operator_delete2, not the actor
       heap: every D0 below ends with a call to 0x0203cbcc. dActor_c's copy of this
       member calls Memory::Deallocate instead, which is why each needs its own.

       Inline, and in the IMMEDIATE base -- mwcc inlines it only when it finds it
       in the class or one level up, as include/dActor_c.h records. No layout
       effect: a non-virtual inline member adds no field and no vtable slot. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

};

typedef char dBgW_KcMbg_size_must_be_0x1c8[sizeof(dBgW_KcMbg) == 0x1c8 ? 1 : -1];

#else

/* The C spelling of the same object, flat -- the arrangement include/ShadowModel.h
   and include/dBgCh_Actr.h already use, and added here for the same reason they
   have one: ten actor headers that a `.c` translation unit reaches embed a
   dBgW_KcMbg BY VALUE, proved by their own destructors calling _ZN10dBgW_KcMbgD1Ev
   at the offset (tools/dtor_members.py). Until this existed those ten members had
   to stay `u8` markers, because C could not name the type at all.

   The base is spelled as a pad rather than `struct dBgW_Kc base;`: dBgW_Kc.h is
   C++-only too, and only the SIZE of the base matters to anything a C file does
   here. Every field below is the C++ declaration above, at the same offset. */
struct dBgW_KcMbg {
    u8  pad_000[0x50];         /* 0x000 - dBgW_Kc base, C++-only above */
    Fix12i scale;              /* 0x050 */
    Matrix4x3 mat;             /* 0x054 */
    Matrix4x3 invRotMat;       /* 0x084 */
    Matrix4x3 scaledMat;       /* 0x0b4 */
    Matrix4x3 invMat;          /* 0x0e4 */
    s16 angY;                  /* 0x114 */
    s16 angVelY;               /* 0x116 */
    Vector3 pos;               /* 0x118 */
    Vector3 velocity;          /* 0x124 */
    u32 unk_130;               /* 0x130 */
    Matrix4x3 newScaledMat;    /* 0x134 */
    s32 invScale;               /* 0x164 */
    Matrix4x3 invScaledMat;    /* 0x168 */
    Matrix4x3 prevInvScaledMat;/* 0x198 */
};

/* In C the tag alone is not a type name, so an owner header that embeds a
   dBgW_KcMbg by value cannot spell the member without this. The definition and
   the typedef have to travel together: with the definition and no typedef the
   embed gets `undefined identifier', and then the owner's size assert gets
   `illegal constant expression' on top of it. */
typedef struct dBgW_KcMbg dBgW_KcMbg;

/* The C view substitutes for the C++ class only while it is the SAME SIZE. Once
   an owner embeds one by value the two branches lay that owner out differently if
   they ever disagree, and nothing else in the build compares them. */
typedef char dBgW_KcMbg_size_must_be_0x1c8[
    sizeof(struct dBgW_KcMbg) == 0x1c8 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DBGW_KCMBG_H */
