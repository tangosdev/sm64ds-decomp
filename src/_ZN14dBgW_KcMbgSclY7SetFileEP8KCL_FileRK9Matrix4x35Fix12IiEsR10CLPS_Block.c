// @symbol _ZN14dBgW_KcMbgSclY7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block
/* recovered: named members + shared header */
struct Matrix4x3;
struct KCL_File;
struct CLPS_Block;
struct dBgW_KcMbgSclY {
    char pad[0x1c8];
    int scaleY;       /* 0x1c8 */
    int invScaleY;    /* 0x1cc */
};
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void* thiz, struct KCL_File* f, const struct Matrix4x3* m, int fix, short s, struct CLPS_Block* b);
void _ZN14dBgW_KcMbgSclY7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(struct dBgW_KcMbgSclY *self, struct KCL_File* f, const struct Matrix4x3* m, int fix, short s, struct CLPS_Block* b) {
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(((void*)self), f, m, fix, s, b);
    *(int*)((char*)&self->scaleY) = 0x1000;
    *(int*)((char*)&self->invScaleY) = 0x1000;
}
