//cpp
// Find the first point at each axis minimum and maximum.
// points must contain at least one point; each output index is optional.
namespace {
// Plain coordinate values: shared Vector3 has nontrivial construction and
// destruction, changing these copies (280 bytes instead of the required 320).
struct Vec3 {
    int x, y, z;
};
}

extern "C" {
void func_ov007_020c2f14(struct Vec3 *points, int pointCount, int *outMinX, int *outMaxX,
                         int *outMinY, int *outMaxY, int *outMinZ, int *outMaxZ) {
    Vec3 maximum = points[0];
    Vec3 minimum = points[0];
    int i;
    int minXIndex, maxXIndex, minYIndex, maxYIndex, minZIndex, maxZIndex;
    maxZIndex = 0;
    minZIndex = 0;
    maxYIndex = 0;
    minYIndex = 0;
    maxXIndex = 0;
    minXIndex = 0;
    for (i = 1; i < pointCount; i++) {
        if (points[i].x < minimum.x) {
            minXIndex = i;
            minimum.x = points[i].x;
        } else if (points[i].x > maximum.x) {
            maxXIndex = i;
            maximum.x = points[i].x;
        }
        if (points[i].y < minimum.y) {
            minYIndex = i;
            minimum.y = points[i].y;
        } else if (points[i].y > maximum.y) {
            maxYIndex = i;
            maximum.y = points[i].y;
        }
        if (points[i].z < minimum.z) {
            minZIndex = i;
            minimum.z = points[i].z;
        } else if (points[i].z > maximum.z) {
            maxZIndex = i;
            maximum.z = points[i].z;
        }
    }

    if (outMinX) {
        *outMinX = minXIndex;
    }
    if (outMinY) {
        *outMinY = minYIndex;
    }
    if (outMinZ) {
        *outMinZ = minZIndex;
    }
    if (outMaxX) {
        *outMaxX = maxXIndex;
    }
    if (outMaxY) {
        *outMaxY = maxYIndex;
    }
    if (outMaxZ) {
        *outMaxZ = maxZIndex;
    }
}
}