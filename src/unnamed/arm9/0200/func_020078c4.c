struct dCamera_c;

extern short ReadUnalignedShort(unsigned char *p);
extern int ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    unsigned char pad_000[0x17a];
    short mFovAngle; /* 0x17a */
} CameraView;

int func_020078c4(struct dCamera_c *self, const unsigned char *data)
{
    CameraView *cam = (CameraView *)self;
    short v = ReadUnalignedShort((unsigned char *)data);
    unsigned char b = data[2];
    ApproachAngle(&cam->mFovAngle, v, b, 0x4000, 0);
    return 1;
}
