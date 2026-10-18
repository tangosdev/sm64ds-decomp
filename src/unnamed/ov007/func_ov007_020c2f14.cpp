//cpp
// Retain the configured address-named export while its callers are reconstructed.
namespace {
// Local coordinate record; this does not introduce a shared Vector3 type.
struct Vec3 {
    int coord[3];
};
}

extern "C" {
void func_ov007_020c2f14(struct Vec3 *points, int count, int *minIdxX, int *maxIdxX, int *minIdxY,
                         int *maxIdxY, int *minIdxZ, int *maxIdxZ) {
    struct Vec3 mx;
    struct Vec3 mn;
    struct Vec3 tmp;
    int i;
    int miX;
    int mxX;
    int miY;
    int mxY;
    int miZ;
    int mxZ;
    tmp = points[0];
    mx = tmp;
    mn = tmp;
    mxZ = 0;
    miZ = 0;
    mxY = 0;
    miY = 0;
    mxX = 0;
    miX = 0;
    for (i = 1; i < count; i++) {
        if (points[i].coord[0] < mn.coord[0]) {
            miX = i;
            mn.coord[0] = points[i].coord[0];
        } else if (points[i].coord[0] > mx.coord[0]) {
            mxX = i;
            mx.coord[0] = points[i].coord[0];
        }
        if (points[i].coord[1] < mn.coord[1]) {
            miY = i;
            mn.coord[1] = points[i].coord[1];
        } else if (points[i].coord[1] > mx.coord[1]) {
            mxY = i;
            mx.coord[1] = points[i].coord[1];
        }
        if (points[i].coord[2] < mn.coord[2]) {
            miZ = i;
            mn.coord[2] = points[i].coord[2];
        } else if (points[i].coord[2] > mx.coord[2]) {
            mxZ = i;
            mx.coord[2] = points[i].coord[2];
        }
    }

    if (minIdxX) {
        *minIdxX = miX;
    }
    if (minIdxY) {
        *minIdxY = miY;
    }
    if (minIdxZ) {
        *minIdxZ = miZ;
    }
    if (maxIdxX) {
        *maxIdxX = mxX;
    }
    if (maxIdxY) {
        *maxIdxY = mxY;
    }
    if (maxIdxZ) {
        *maxIdxZ = mxZ;
    }
}
}