// @symbol _ZN21ExtendingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block
/* recovered: named members + shared header */
#include "ExtendingMeshCollider.h"
struct Matrix4x3;
struct KCL_File;
struct CLPS_Block;
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void* thiz, struct KCL_File* f, const struct Matrix4x3* m, int fix, short s, struct CLPS_Block* b);
void _ZN21ExtendingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(struct ExtendingMeshCollider *self, struct KCL_File* f, const struct Matrix4x3* m, int fix, short s, struct CLPS_Block* b) {
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((void*)self), f, m, fix, s, b);
    *(int*)((char*)&self->unk_1c8) = 0x1000;
    *(int*)((char*)&self->unk_1cc) = 0x1000;
}
