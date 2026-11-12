#ifndef OV007_CAMERA_H
#define OV007_CAMERA_H

#include "types.h"
#include "math/Matrix.h"
#include "Ov007Tween.h"
#include "ds_width.h"

/* Reconstructed layout and name, not recovered class identity. The 0xd8-byte
 * camera func_ov007_020c023c allocates (one per screen, Ov007Work::cameras)
 * and func_ov007_020c0128 resets:
 *
 * - func_ov007_020bffb8 places the eye `distance` from the target at
 *   elevation/azimuth.
 * - func_ov007_020b2160 passes eye/up/target to G3i_LookAt_ (into
 *   viewMatrix), and fovy/aspect/nearZ/farZ/scaleW to G3i_PerspectiveW_
 *   (into projMatrix).
 * - func_ov007_020bff38 steps the three tweens while their `tweening` bits
 *   are set.
 *
 * word00, half34, wordCC and wordD0 are written only by
 * func_ov007_020c0128; no reader names them. */
typedef struct Ov007Camera {
    int word00;               /* 0x00: cleared on reset; no reader found */
    unsigned int tweening;    /* 0x04: bit 0 eye, bit 1 target, bit 2 distance */
    Vector3 eye;              /* 0x08 */
    Vector3 target;           /* 0x14 */
    Vector3 up;               /* 0x20 */
    int distance;             /* 0x2c */
    unsigned short elevation; /* 0x30 */
    unsigned short azimuth;   /* 0x32 */
    unsigned short half34;    /* 0x34: cleared beside the two angles; no reader found */
    char pad_36[2];
    Ov007VecTween *eyeTween;    /* 0x38: func_ov007_020c8f64, bound to eye */
    Ov007VecTween *targetTween; /* 0x3c: func_ov007_020c8f64, bound to target */
    Ov007Tween *distanceTween;  /* 0x40: func_ov007_020c908c, bound to distance */
    Matrix4x3 viewMatrix;       /* 0x44 */
    int projMatrix[16];         /* 0x74: a 4x4 */
    int scaleW;                 /* 0xb4: PerspectiveW_'s scale */
    /* 0xb8..0xbb: the screen rectangle in pixels; func_ov007_020c0128 sets
     * the whole 256x192 screen, func_ov007_020c510c unprojects its corners,
     * and the viewport register takes minX | minY << 8 | maxX << 16 | maxY << 24 */
    unsigned char viewportMinX;
    unsigned char viewportMaxX;
    unsigned char viewportMinY;
    unsigned char viewportMaxY;
    long long aspect; /* 0xbc: PerspectiveW_ takes it >> 0x14 */
    int nearZ;        /* 0xc4 */
    int farZ;         /* 0xc8 */
    int wordCC;       /* 0xcc: 0x1000 on reset; no reader found */
    int wordD0;       /* 0xd0: 0xc00 on reset; no reader found */
    /* 0xd4: a u16 angle; the sine table is indexed with it >> 4 */
    unsigned short fovy;
} Ov007Camera;

#ifndef SM64DS_PLATFORM_PC
/* aspect at 0xbc relies on long long taking 4-byte alignment here. */
typedef char Ov007Camera_aspect_must_be_at_0xbc[DS_IADDR(&((Ov007Camera *)0)->aspect) == 0xbc ? 1 : -1];
typedef char Ov007Camera_fovy_must_be_at_0xd4[DS_IADDR(&((Ov007Camera *)0)->fovy) == 0xd4 ? 1 : -1];
typedef char Ov007Camera_size_must_be_0xd8[sizeof(Ov007Camera) == 0xd8 ? 1 : -1];
#endif

#endif
