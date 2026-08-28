/* HOST COPY of src/_ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3.cpp
 *
 * Same reason as the PerspectiveW_ copy next to it: the view matrix is
 * pushed through a register-bound pointer --
 *
 *     mtx = (volatile int *)0x400045c;     // MTX_LOAD_4x3
 *     *mtx = right.x;  ...
 *
 * -- which hostgen.py's literal-address rewrite cannot see, so all twelve
 * words land in the mapped I/O window instead of the geometry engine and the
 * camera transform never reaches the raster. Each store goes through
 * NTR_MMIO here. The math is the matched source verbatim.
 *
 * Argument names follow the decomp's: `at` is the point the matrix is built
 * about (Camera::Render passes the LOOK-AT point as `at` and the EYE as
 * `eye`, which is why the forward vector comes out reversed relative to the
 * usual convention -- the ROM's own sign, left alone).
 */
#include "ntr/mmio.h"

#include <cstdio>
#include <cstdlib>

struct Vector3 { int x, y, z; };
struct Matrix4x3_ { int m[12]; };

extern "C" void NormalizeVec3(Vector3 *v, Vector3 *out);
extern "C" void CrossVec3(const Vector3 *a, const Vector3 *b, Vector3 *out);
extern "C" int DotVec3(const Vector3 *a, const Vector3 *b);

/* PORT_HOST_ABI: the matrix payload leaves through a register-bound
 * pointer into the GX MMIO port, which hostgen's literal-address rewrite
 * cannot see. See the header. */
extern "C" void _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(
    const Vector3 *at, const Vector3 *up, const Vector3 *eye, char draw,
    Matrix4x3_ *mat)
{
    Vector3 forward;
    Vector3 right;
    Vector3 up2;
    int tx, ty, tz;

    forward.x = at->x - eye->x;
    forward.y = at->y - eye->y;
    forward.z = at->z - eye->z;
    NormalizeVec3(&forward, &forward);

    CrossVec3(up, &forward, &right);
    NormalizeVec3(&right, &right);

    CrossVec3(&forward, &right, &up2);

    if (draw) {
        NTR_MMIO(int, 0x4000440) = 2;          /* MTX_MODE position+vector */
        /* MTX_LOAD_4x3: nine rotation words then the translation row */
        NTR_MMIO(int, 0x400045c) = right.x;
        NTR_MMIO(int, 0x400045c) = up2.x;
        NTR_MMIO(int, 0x400045c) = forward.x;
        NTR_MMIO(int, 0x400045c) = right.y;
        NTR_MMIO(int, 0x400045c) = up2.y;
        NTR_MMIO(int, 0x400045c) = forward.y;
        NTR_MMIO(int, 0x400045c) = right.z;
        NTR_MMIO(int, 0x400045c) = up2.z;
        NTR_MMIO(int, 0x400045c) = forward.z;
    }

    tx = -DotVec3(at, &right);
    ty = -DotVec3(at, &up2);
    tz = -DotVec3(at, &forward);

    if (draw) {
        NTR_MMIO(int, 0x400045c) = tx;
        NTR_MMIO(int, 0x400045c) = ty;
        NTR_MMIO(int, 0x400045c) = tz;
    }

    /* SM64DS_LOOKAT_LOG=<n>: the first n calls' three vectors and the
       translation row they produce. DEFAULT OFF, PerspectiveW_'s SM64DS_
       PERSP_LOG next door is the same instrument for the other half of the
       camera.
       tz IS THE ONE TO READ. It is the eye-space depth of the world origin,
       so a 2D-in-3D surface drawn about the origin sits at -tz, and the
       number of screen pixels one of its texels covers is decided by tz and
       the projection's cotangent together. A UI that comes out oversized and
       a camera that is too close are the same measurement, and without this
       they are two arguments. */
    {
        static int budget = -1;
        if (budget < 0) {
            const char *e = std::getenv("SM64DS_LOOKAT_LOG");
            budget = e ? std::atoi(e) : 0;
        }
        if (budget > 0) {
            --budget;
            std::fprintf(stderr,
                         "[lookat] at=(%d,%d,%d)=(%.4f,%.4f,%.4f) "
                         "eye=(%d,%d,%d) up=(%d,%d,%d) draw=%d -> "
                         "fwd=(%.4f,%.4f,%.4f) t=(%.4f,%.4f,%.4f)\n",
                         at->x, at->y, at->z, at->x / 4096.0, at->y / 4096.0,
                         at->z / 4096.0, eye->x, eye->y, eye->z, up->x, up->y,
                         up->z, (int)draw, forward.x / 4096.0,
                         forward.y / 4096.0, forward.z / 4096.0, tx / 4096.0,
                         ty / 4096.0, tz / 4096.0);
            std::fflush(stderr);
        }
    }

    if (mat == 0)
        return;

    mat->m[0] = right.x;
    mat->m[1] = up2.x;
    mat->m[2] = forward.x;
    mat->m[3] = right.y;
    mat->m[4] = up2.y;
    mat->m[5] = forward.y;
    mat->m[6] = right.z;
    mat->m[7] = up2.z;
    mat->m[8] = forward.z;
    mat->m[9] = tx;
    mat->m[10] = ty;
    mat->m[11] = tz;
}
