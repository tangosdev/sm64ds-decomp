//cpp
#pragma opt_propagation off
struct Vector3 { int x, y, z; };
namespace cstd { int atan2(int y, int x); }
extern "C" int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern "C" void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, Vector3 *v, unsigned int i);
extern "C" int func_ov062_02119af0(char *p) {
    int dxc;
    int dzc;
    int dx;
    int dz;
    int ncc;
    int n5c;
    int nc0;
    int nd4;
    int n64;
    int nc8;
    
    ncc = *(int *)(p + 0x3cc);
    n5c = *(int *)(p + 0x5c);
    nc0 = *(int *)(p + 0x3c0);
    dx = ncc - n5c;
    nd4 = *(int *)(p + 0x3d4);
    n64 = *(int *)(p + 0x64);
    nc8 = *(int *)(p + 0x3c8);
    dz = nd4 - n64;
    dxc = ncc - nc0;
    dzc = nd4 - nc8;
    *(short *)(p + 0x3a8) = (short)cstd::atan2(dx, dz);
    int dot = (dxc >> 0xc) * (dx >> 0xc) + (dzc >> 0xc) * (dz >> 0xc);
    if (dot <= 0 || Vec3_Dist((Vector3 *)(p + 0x5c), (Vector3 *)(p + 0x3cc)) < (*(int *)(p + 0x98) >> 1)) {
        int v = *(int *)(p + 0x3cc);
        int *cnt = (int *)(((long long)(int)(p + 0x3bc)) & 0xFFFFFFFFFFFFFFFFLL);
        *(int *)(p + 0x3c0) = v;
        *(int *)(p + 0x3c4) = *(int *)(p + 0x3d0);
        *(int *)(p + 0x3c8) = *(int *)(p + 0x3d4);
        *cnt = *cnt + 1;
        if (*(int *)(p + 0x3bc) >= *(int *)(p + 0x3b8)) {
            *(int *)(p + 0x3bc) = 0;
        }
        int idx = *(int *)(p + 0x3bc);
        if (idx == 0) return -1;
        _ZNK7PathPtr7GetNodeER7Vector3j(p + 0x3d8, (Vector3 *)(p + 0x3cc), (unsigned)idx);
        return 1;
    }
    return 0;
}
