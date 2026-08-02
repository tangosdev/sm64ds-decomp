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

void ModelComponents::Render(Matrix4x3 *mat, Vector3 *vec)
{
    data_020a4bd4 = 1u << (info->field0 + 12);
    if (!mat) {
        data_020a4bd0 = &data_02082190;
    } else {
        data_020a4bd0 = mat;
    }

    u32 i = 0;
    Node *node = info->nodes;
    for (; i < info->count; ++i, ++node) {
        if (node->flags & 1) {
            for (int j = 0; j < node->innerCount; j++) {
                u8 idx = node->arrA[j];
                if (!((modules[idx].flags >> 31) & 1)) {
                    func_02044b30(this, idx);
                    func_02044534(this, node->arrB[j], mat, vec);
                }
            }
        } else {
            for (int j = 0; j < node->innerCount; j++) {
                u8 idx = node->arrA[j];
                if (!(modules[idx].flags & 0x80000000)) {
                    func_02044b30(this, idx);
                    func_0204488c(this, node->arrB[j], vec);
                }
            }
        }
    }
}
