struct S;
extern unsigned short ReadUnalignedUshort(unsigned char *p);

/* File-local view of dCamera_c (include/dCamera_c.h is C++). */
typedef struct {
    unsigned char pad_000[0x17a];
    unsigned short mFovAngle; /* 0x17a */
} CameraView;

int func_02008450(struct S *p, int arg1)
{
    CameraView *cam = (CameraView *)p;
    unsigned short v = ReadUnalignedUshort((unsigned char *)arg1);
    cam->mFovAngle = v;
    return 1;
}
