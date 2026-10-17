extern void func_0200cb58(void *obj, int index);
extern void func_0200cce4(void *obj);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    char pad_000[0x154];
    int mFlags; /* 0x154 */
} CameraView;

int func_020050dc(void *obj)
{
    CameraView *cam = (CameraView *)obj;
    func_0200cb58(obj, 0x1e);
    func_0200cce4(obj);
    cam->mFlags |= 0x100;
    return 1;
}
