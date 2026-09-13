// HOST REIMPLEMENTATION -- not byte-verified against the ROM.
//
// The matched src/_ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3.cpp calls
// the material bind as `func_02044b30(this)`: on the DS the material index
// travels to the callee in r1, left there by the loop's own arithmetic -- a
// register contract the byte-matched C never names (same class as
// SharedFilePtr::Construct's r1 ride-through). On a host ABI that argument
// must be passed, so this copy spells it out. Behavior is otherwise the
// matched walk, line for line.
#include "types.h"

extern "C" {
extern u32 data_020a4bd4;
extern int data_02082190;
extern void *data_020a4bd0;

struct Matrix4x3;
struct Vector3;

struct Node {
    u8 pad[0x30];
    u16 innerCount;
    u8 pad2[2];
    u8 *arrA;
    u8 *arrB;
    u16 flags;
    u8 pad3[2];
};

struct Info {
    u8 field0;
    u8 pad[3];
    u32 count;
    Node *nodes;
};

struct ModuleEntry {
    u8 pad[0x24];
    u32 flags;
    u8 pad2[8];
};

struct ModelComponents {
    Info *info;
    ModuleEntry *modules;

    void Render(Matrix4x3 *mat, Vector3 *vec);
};

extern void func_02044b30(ModelComponents *self, int idx);   /* material bind */
extern void func_02044534(ModelComponents *self, int idx, Matrix4x3 *mat, Vector3 *vec);
extern void func_0204488c(ModelComponents *self, int idx, Vector3 *vec);
}

/* ModelComponents::Render RETIRED (run link100, lane SEAT6, batch B6).
   This file is a host REIMPLEMENTATION and says so in its first line
   ('not byte-verified against the ROM'). Its one real divergence is the
   material index the ROM leaves in r1, which src/func_02044b30.c's own
   definition takes as its second parameter.
   The matched TU src/_ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3.cpp is seated in its place: port/tools/hostgen.py's REG_RIDE_ARG table gives the declaration
   `int idx` and both call sites the loop's own idx. The cdecl C-name
   face in port/hal/scene_boot.cpp is untouched and stays.
   Per-row ROM evidence (referrer, RTTI name, kind:function record, the
   dispatch instruction read at its own address) is in port/slice_seat6.txt. */
