extern int DecIfAbove0_Short(void *p);
extern void func_0200cb58(void *obj, int index);
extern void func_02009e70(void *obj);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    unsigned char pad_000[0x196];
    /* func_020098b8 arms it (0x1c) on entering this state; this state counts
       it down and switches to state 0x14 when it runs out. */
    short mStateTimer; /* 0x196 */
} CameraView;

void func_0200987c(void *self) {
    CameraView *cam = (CameraView *)self;
    if (DecIfAbove0_Short(&cam->mStateTimer) == 0) {
        func_0200cb58(self, 0x14);
    }
    func_02009e70(self);
}
