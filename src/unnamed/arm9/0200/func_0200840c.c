struct dCamera_c;

extern short ReadUnalignedShort(unsigned char *p);
extern short _Z15ApproachLinear2Rsss(short *ref, short a, short b);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    unsigned char pad_000[0x17a];
    short mFovAngle; /* 0x17a */
} CameraView;

int func_0200840c(struct dCamera_c *cam, const char *data) {
    CameraView *view = (CameraView *)cam;
    short v1 = ReadUnalignedShort((unsigned char *)data);
    short v2 = ReadUnalignedShort((unsigned char *)data + 2);
    _Z15ApproachLinear2Rsss(&view->mFovAngle, v1, v2);
    return 1;
}
