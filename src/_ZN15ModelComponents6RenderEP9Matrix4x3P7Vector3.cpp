//cpp
extern "C" {

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern u32 data_020a4bd4;
extern int data_02082190;
extern void* data_020a4bd0;

struct Matrix4x3;
struct Vector3;

struct Node {
    u8 pad[0x30];
    u16 innerCount;
    u8 pad2[2];
    u8* arrA;
    u8* arrB;
    u16 flags;
    u8 pad3[2];
};

struct Info {
    u8 field0;
    u8 pad[3];
    u32 count;
    Node* nodes;
};

struct ModuleEntry {
    u8 pad[0x24];
    u32 flags;
    u8 pad2[8];
};

struct ModelComponents {
    Info* info;
    ModuleEntry* modules;

    void Render(Matrix4x3* mat, Vector3* vec);
};

extern void func_02044b30(ModelComponents* self);
extern void func_02044534(ModelComponents* self, int idx, Matrix4x3* mat, Vector3* vec);
extern void func_0204488c(ModelComponents* self, int idx, Vector3* vec);

void ModelComponents::Render(Matrix4x3* mat, Vector3* vec) {
    data_020a4bd4 = 1u << (info->field0 + 12);
    if (!mat) {
        data_020a4bd0 = &data_02082190;
    } else {
        data_020a4bd0 = mat;
    }

    u32 i = 0;
    Node* node = info->nodes;
    if (i < info->count) {
        do {
            if (node->flags & 1) {
                for (int j = 0; j < node->innerCount; j++) {
                    u8 idx = node->arrA[j];
                    u32 hidden = (modules[idx].flags >> 31) & 1;
                    if (!hidden) {
                        func_02044b30(this);
                        func_02044534(this, node->arrB[j], mat, vec);
                    }
                }
            } else {
                for (int j = 0; j < node->innerCount; j++) {
                    u8 idx = *(volatile u8*)&node->arrA[j];
                    ModuleEntry* m = modules;
                    m += idx;
                    if (!(m->flags & 0x80000000)) {
                        func_02044b30(this);
                        func_0204488c(this, node->arrB[j], vec);
                    }
                }
            }
            i++;
            node++;
        } while (i < info->count);
    }
}

}
