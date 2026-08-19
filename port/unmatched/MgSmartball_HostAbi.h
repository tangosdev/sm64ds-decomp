/* PORT_HOST_ABI. Two host-side declarations for dScMgSmartball_c's closure,
 * force-included ahead of the two src/ translation units that need them so
 * neither src file has to be edited. Run mg5, lane SMBSEAT.
 *
 * Both entries below shadow a real include/ header by pre-defining that
 * header's own guard macro. That is the whole mechanism: the src file still
 * says `#include "..."`, the include still happens, and the body of the header
 * is skipped because its guard is already defined. Nothing in src/ or include/
 * changes, and the byte-gated tree keeps compiling the same two headers it
 * always did.
 *
 * ---- 1. DispObj: THE SUB-OBJECT DISPATCH CONVENTION ----------------------
 *
 * include/private/disp_obj_vtbl.h declares the smartball sub-object's two
 * virtuals as ordinary C++ members. On MSVC an ordinary member is __thiscall,
 * so `p->f1()` puts the receiver in ECX. EVERY OTHER DISPATCH THROUGH THE SAME
 * TWELVE VTABLES IN THIS BINARY IS CDECL: src/func_ov006_02118488.c, the
 * class's own Behavior, declares `typedef void (*VFunc)(void*)` and makes
 * twelve `(**(VFunc**)o)(o)` calls, which push the receiver. A vtable cannot
 * hold both shapes at once, so the port picks the majority and makes the odd
 * caller match it.
 *
 * THIS IS THE SAME RULING port/CMakeLists.txt ALREADY MAKES FOR func_02021d1c,
 * the particle-spawn tail: "the ONE place in the subsystem that reaches a
 * Callback vtable through a C++ `virtual` -- a local shadow class, `p6->Run`,
 * which MSVC compiles thiscall while every other dispatch on the same vtable
 * is cdecl". That case is routed through hostgen's VIRTUAL_CALL patch, which
 * rewrites the call expression. This one needs no rewrite: MSVC accepts an
 * explicit calling convention on a virtual member, so declaring the two slots
 * __cdecl makes `p->f1()` compile to `push p; call [vptr+4]; add esp,4` -- the
 * identical instruction shape the VFunc callers already emit. The vtable
 * layout is unchanged; only the convention on the way in is.
 *
 * ---- 2. dScMgSmartball_c: A LAYOUT CLAIM THE HOST CANNOT MEET ------------
 *
 * include/dScMgSmartball_c.h ends with
 *
 *     typedef char dScMgSmartball_c_size_must_be_0x629c[
 *         sizeof(dScMgSmartball_c) == 0x629c ? 1 : -1];
 *
 * and that assert is CORRECT for mwccarm and cannot hold on MSVC. Measured in
 * this lane rather than assumed: sizeof(dScMgBase_c) is 0x4660 on both, but
 * the base's last field is a u8 at 0x465c, so its data size is 0x465d and the
 * Itanium ABI starts the derived class's own members THERE, inside the base's
 * tail padding -- which is why the real header spells its first member
 * `pad_465d`. MSVC does not reuse base tail padding, and because dScMgBase_c
 * declares no virtual of its own MSVC also inserts a fresh vfptr at offset 0
 * for the derived class's `virtual ~dScMgSmartball_c()`. A host build of the
 * real header measures 0x47d0 where the ROM has 0x47c8 for the same prefix:
 * eight bytes of divergence, four from the vfptr and four from the padding.
 *
 * IT COSTS THE PORT NOTHING, and that is the point of shadowing it rather than
 * fighting it. src/_ZN16dScMgSmartball_c6RenderEv.cpp opens with
 * `char *g = (char *)this;` and reaches every field it touches as a raw offset
 * off g. `this` is the object address under either ABI, so every one of those
 * offsets is right on the host; not one member of this class is named. The
 * declaration below therefore keeps the class NAME and the three method
 * signatures -- which is all MSVC's mangling encodes, so the definition in
 * src/ and the forwarder in MgSmartball_Faces.cpp still agree on
 * ?Render@dScMgSmartball_c@@QAEHXZ -- and drops both the fields and the
 * virtual destructor, so no vfptr is inserted and nothing writes one.
 *
 * The sub-object classes need NO such treatment and are included for real:
 * cMgSmartball_object_c is a genuine polymorphic root, its vptr sits at offset
 * 0 under both ABIs, and both its 0x34 assert and cMgSmartball_board_c's 0x9c
 * assert compile clean on MSVC. Only the tail-padding case above diverges.
 */
#ifndef PORT_MGSMARTBALL_HOSTABI_H
#define PORT_MGSMARTBALL_HOSTABI_H

/* 1. shadows include/private/disp_obj_vtbl.h */
#define DISP_OBJ_VTBL_H
struct DispObj {
    virtual void __cdecl f0();
    virtual void __cdecl f1();
};

/* 2. shadows include/dScMgSmartball_c.h */
#define DSCMGSMARTBALL_C_H
#include "dScMgBase_c.h"
struct dScMgSmartball_c : dScMgBase_c {
    void AfterCleanupResources(u32 vfSuccess);  /* slot  5 */
    s32  Behavior();                            /* slot  6 */
    s32  Render();                              /* slot  9 */
};

#endif
