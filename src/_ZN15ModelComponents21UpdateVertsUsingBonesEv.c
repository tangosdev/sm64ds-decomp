#include "types.h"
struct BMD_File {
    u32 scaleShift;  /* 0x00 */
    u32 numBones;    /* 0x04 */
    void* bones;     /* 0x08 */
};

struct ModelComponents {
    struct BMD_File* modelFile; /* 0x00 */
    void* materials;            /* 0x04 */
    void* bones;                /* 0x08 */
    void* transforms;           /* 0x0c */
    void* unk10;
};

extern void Matrix4x3_LoadIdentity(void*);
extern void func_02045074(struct ModelComponents*, void*);

void _ZN15ModelComponents21UpdateVertsUsingBonesEv(struct ModelComponents* this) {
    Matrix4x3_LoadIdentity(this->transforms);
    func_02045074(this, this->modelFile->bones);
}
