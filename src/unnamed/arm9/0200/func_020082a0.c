extern unsigned short data_0209b274;
extern int func_020094a8(void *p);
extern unsigned int ReadUnalignedInt(unsigned char *p);
extern void func_02009414(void *p);

/* File-local view of dCamera_c (include/dCamera_c.h is C++): the fields this
 * script op touches. Both are paths func_02009414 walks with func_02008b4c --
 * lookAt along mLookAtPath, pos along mPosPath -- after func_020094a8 has
 * reset the shared segment index and progress. */
typedef struct {
    unsigned char pad_000[0x14c];
    int mLookAtPath; /* 0x14c */
    int mPosPath;    /* 0x150 */
} CameraView;

int func_020082a0(void *self, const void *src, int id)
{
    CameraView *cam = (CameraView *)self;
    if (id == data_0209b274) {
        func_020094a8(self);
        cam->mLookAtPath = (int)ReadUnalignedInt((unsigned char *)src);
        cam->mPosPath = (int)ReadUnalignedInt((unsigned char *)src + 4);
    }
    func_02009414(self);
    return 1;
}
