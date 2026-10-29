//cpp
#include "G3i.h"

extern "C" {

/* func_ov007_020c0078 at 0x020c0078 (ov007)
 *
 * Ov007Camera perspective setup: writes the viewport register (0x4000580)
 * from the camera's screen rectangle, then calls G3i::PerspectiveW_ with a
 * sine-table pair for fovy, aspect >> 20, nearZ and farZ into projMatrix,
 * and finally clears 0x400044c.
 *
 * Matched byte-for-byte with mwccarm 2004/b56.
 */
extern short data_02082214[];

/* The 0xd8-byte camera func_ov007_020c023c allocates; names as in
 * func_ov007_020c0128, only the slots this file reads. */
typedef struct Ov007Camera {
    char pad_00[0x74];
    int projMatrix[16]; /* 0x74: Matrix4x4 */
    char pad_b4[4];
    /* 0xb8..0xbb: the screen rectangle in pixels; the viewport register
     * takes minX | minY << 8 | maxX << 16 | maxY << 24 */
    unsigned char viewportMinX;
    unsigned char viewportMaxX;
    unsigned char viewportMinY;
    unsigned char viewportMaxY;
    long long aspect; /* 0xbc: PerspectiveW_ takes it >> 0x14 */
    int nearZ;        /* 0xc4 */
    int farZ;         /* 0xc8 */
    char pad_cc[8];
    /* 0xd4: a u16 angle; the sine table is indexed with it >> 4 */
    unsigned short fovy;
} Ov007Camera;

/* aspect at 0xbc relies on long long taking 4-byte alignment here. */
typedef char Ov007Camera_aspect_must_be_at_0xbc[(int)&((Ov007Camera *)0)->aspect == 0xbc ? 1 : -1];
typedef char Ov007Camera_fovy_must_be_at_0xd4[(int)&((Ov007Camera *)0)->fovy == 0xd4 ? 1 : -1];

/* c keeps the char * spelling of the definition; `cam` is the typed view. */
void func_ov007_020c0078(char* c)
{
    Ov007Camera *cam = (Ov007Camera *)c;
    int a;
    *(int *)0x4000580 = cam->viewportMinX | (cam->viewportMinY << 8) | (cam->viewportMaxX << 16) |
                        (cam->viewportMaxY << 24);

    a = (int)cam->fovy >> 4;
    G3i::PerspectiveW_(data_02082214[a * 2], data_02082214[a * 2 + 1], (int)(cam->aspect >> 0x14),
                       cam->nearZ, cam->farZ, 0x1000, 1, (Matrix4x3 *)cam->projMatrix);

    *(int*)0x400044c = 0;
}
}
