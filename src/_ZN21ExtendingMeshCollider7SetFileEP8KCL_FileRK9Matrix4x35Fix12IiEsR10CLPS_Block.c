struct Matrix4x3;
struct KCL_File;
struct CLPS_Block;
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void* thiz, struct KCL_File* f, const struct Matrix4x3* m, int fix, short s, struct CLPS_Block* b);
void _ZN21ExtendingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void* thiz, struct KCL_File* f, const struct Matrix4x3* m, int fix, short s, struct CLPS_Block* b) {
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(thiz, f, m, fix, s, b);
    *(int*)((char*)thiz + 0x1c8) = 0x1000;
    *(int*)((char*)thiz + 0x1cc) = 0x1000;
}
