extern void _Z14ApproachLinearRiii(int *p, int value, int speed);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    int x, y, z;
} Vec3;

typedef struct {
    unsigned char pad_000[0xb0];
    Vec3 savedLookAt; /* 0x0b0 */
    Vec3 savedPos;    /* 0x0bc */
} CameraView;

int func_020072c4(char *self)
{
    CameraView *cam = (CameraView *)self;
    /* Camera word 0x174 stays raw: this state eases it up and paces the
       saved pair by it, but no other reader confirms the role. */
    _Z14ApproachLinearRiii((int *)(self + 0x174), 0xf000, 0x199);
    _Z14ApproachLinearRiii(&cam->savedLookAt.x, 0x7d0000, *(int *)(self + 0x174));
    _Z14ApproachLinearRiii(&cam->savedLookAt.y, 0x4b0000, *(int *)(self + 0x174) / 10);
    _Z14ApproachLinearRiii(&cam->savedLookAt.z, 0x3e8000, *(int *)(self + 0x174) / 10);
    _Z14ApproachLinearRiii(&cam->savedPos.x, 0, *(int *)(self + 0x174));
    _Z14ApproachLinearRiii(&cam->savedPos.y, 0x4b0000, *(int *)(self + 0x174) >> 1);
    _Z14ApproachLinearRiii(&cam->savedPos.z, 0x3e8000, *(int *)(self + 0x174) * 10 / 15);
    return 1;
}
