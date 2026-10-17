struct S;

extern void func_0200cb58(void *obj, int index);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    unsigned char pad_000[0x196];
    /* Armed here on entering state 0x13; func_0200987c counts it down and
       switches to state 0x14 when it runs out. */
    unsigned short mStateTimer; /* 0x196 */
} CameraView;

int func_020098b8(struct S *p)
{
    CameraView *cam = (CameraView *)p;
    func_0200cb58(p, 0x13);
    cam->mStateTimer = 0x1c;
    return 1;
}
