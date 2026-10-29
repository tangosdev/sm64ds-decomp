//cpp
#include "types.h"
#include "cstd.h"
extern "C" void Matrix4x3_LoadIdentity(void *m);
extern "C" void func_02052ec8(void *p);

/* The 0xd8-byte camera func_ov007_020c023c allocates and this function
 * resets. func_ov007_020b2160 passes eye/up/target to G3i::LookAt_ (into
 * viewMatrix) and fovy/aspect/nearZ/farZ/scaleW to G3i::PerspectiveW_ (into
 * projMatrix); func_ov007_020bffb8 places the eye `distance` from the target
 * at elevation/azimuth; func_ov007_020bff38 runs the three tweens while their
 * `tweening` bits are set. Only the slots spelled here have a reader.
 *
 * Left raw, on the char * `c`: +0x00, the u16 at +0x34 (zeroed beside the
 * two angles) and the ints at +0xcc and +0xd0; no reader names them. */
struct Ov007Camera {
    char pad_00[4];
    unsigned int tweening;    /* 0x04: bit 0 eye, bit 1 target, bit 2 distance */
    Vector3 eye;              /* 0x08 */
    Vector3 target;           /* 0x14 */
    Vector3 up;               /* 0x20 */
    int distance;             /* 0x2c */
    unsigned short elevation; /* 0x30 */
    unsigned short azimuth;   /* 0x32 */
    char pad_34[0x10];
    int viewMatrix[12]; /* 0x44: Matrix4x3 */
    int projMatrix[16]; /* 0x74: Matrix4x4 */
    int scaleW;         /* 0xb4 */
    /* 0xb8..0xbb: the screen rectangle in pixels, the whole 256x192 screen
     * here; func_ov007_020c510c unprojects its corners */
    unsigned char viewportMinX;
    unsigned char viewportMaxX;
    unsigned char viewportMinY;
    unsigned char viewportMaxY;
    long long aspect; /* 0xbc: PerspectiveW_ takes it >> 0x14 */
    int nearZ;        /* 0xc4 */
    int farZ;         /* 0xc8 */
    char pad_cc[8];
    /* 0xd4: a u16 angle; func_ov007_020b2160 indexes the sine table with it >> 4 */
    short fovy;
};

/* aspect at 0xbc relies on long long taking 4-byte alignment here. */
typedef char Ov007Camera_aspect_must_be_at_0xbc[(int)&((Ov007Camera *)0)->aspect == 0xbc ? 1 : -1];
typedef char Ov007Camera_size_must_be_0xd8[sizeof(Ov007Camera) == 0xd8 ? 1 : -1];

extern "C" void func_ov007_020c0128(char *c)
{
    Ov007Camera *cam = (Ov007Camera *)c;
    cam->eye.y = 0;
    cam->eye.x = cam->eye.y;
    cam->eye.z = 0x1000;
    cam->target.z = 0;
    cam->target.y = cam->target.z;
    cam->target.x = cam->target.y;
    cam->distance = 0;
    *(unsigned short *)(c + 0x34) = 0;
    cam->azimuth = *(unsigned short *)(c + 0x34);
    cam->elevation = cam->azimuth;
    cam->up.z = 0;
    cam->up.x = cam->up.z;
    cam->up.y = 0x1000;
    cam->nearZ = 0x1000;
    cam->farZ = 0x190000;
    cam->fovy = (short)cstd::div(0x1e0000, 0x168);
    long long l = cstd::ldiv(0x4000, 0x3000);
    cam->aspect = l;
    cam->viewportMinX = 0;
    cam->viewportMaxX = 0xff;
    cam->viewportMinY = 0;
    cam->viewportMaxY = 0xbf;
    *(int *)(c + 0xcc) = 0x1000;
    *(int *)(c + 0xd0) = 0xc00;
    Matrix4x3_LoadIdentity(cam->viewMatrix);
    func_02052ec8(cam->projMatrix);
    cam->scaleW = 0x1000;
    *(int *)(c + 0) = 0;
    cam->tweening = 0;
}
