//cpp
extern "C" {

typedef struct { int x, y, z; } Vector3;

extern int data_0209f43c;
extern int data_0209b3ec;
extern int data_0209b41c;

extern int _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(void *thiz, void *mat, Vector3 *srcVec, int scale, Vector3 *dstVec);
extern void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *dst);

int _ZN8Particle12ClipCallback8OnUpdateERNS_6SystemEb(char *self, char *sys, int isFlag)
{
    char *node;
    Vector3 wp, cv, out;
    int result;

    if (*(unsigned char *)(self + 4) == 0) {
        *(int *)(((long long)(int)(sys + 0x1c)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
        node = *(char **)(sys + 8);
        while (node != 0) {
            *(unsigned short *)(node + 0x2e) = *(unsigned short *)(node + 0x2c);
            node = *(char **)node;
        }
        return 1;
    } else {
        *(int *)(((long long)(int)(sys + 0x1c)) & 0xFFFFFFFFFFFFFFFFLL) &= ~2;
        node = *(char **)(sys + 8);
        while (node != 0) {
            wp.x = *(int *)(node + 0x14) + *(int *)(node + 0x8);
            wp.y = *(int *)(node + 0x18) + *(int *)(node + 0xc);
            wp.z = *(int *)(node + 0x1c) + *(int *)(node + 0x10);

            result = _ZN7Clipper13Func_02015560ER9Matrix4x3R7Vector35Fix12IiES3_(&data_0209f43c, &data_0209b3ec, &wp, 0x8000, &cv);
            if (result > 0x100000) {
                if (cv.y < -0x40000) {
                    *(unsigned short *)(node + 0x2e) = *(unsigned short *)(node + 0x2c);
                } else {
                    if (cv.y > 0x40000) {
                        cv.y -= 0x50000;
                    } else if (cv.z > 0) {
                        cv.z -= 0x80000;
                    } else {
                        cv.x = (cv.x < 0) ? 0x20000 : -0x20000;
                    }
                    MulVec3Mat4x3(&cv, &data_0209b41c, &out);
                    *(int *)(((long long)(int)(node + 0x14)) & 0xFFFFFFFFFFFFFFFFLL) += out.x - wp.x;
                    *(int *)(((long long)(int)(node + 0x18)) & 0xFFFFFFFFFFFFFFFFLL) += out.y - wp.y;
                    *(int *)(((long long)(int)(node + 0x1c)) & 0xFFFFFFFFFFFFFFFFLL) += out.z - wp.z;
                }
            }
            node = *(char **)node;
        }
        return 1;
    }
}

}
