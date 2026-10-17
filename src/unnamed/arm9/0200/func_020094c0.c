extern void func_02009e70(void *obj);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    unsigned char pad_000[0x110];
    int mTargetPlayer; /* 0x110 */
    unsigned char pad_114[0x40];
    unsigned int mFlags; /* 0x154 */
} CameraView;

void func_020094c0(void *self)
{
    CameraView *cam = (CameraView *)self;
    /* 0x118 stays raw: Behavior re-sets flag 4 while it is nonzero and then
       clears it, but nothing here shows what the copied word itself means. */
    *(unsigned int *)((char *)self + 0x118) = cam->mTargetPlayer;
    cam->mFlags |= 4;
    func_02009e70(self);
}
