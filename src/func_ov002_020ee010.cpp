//cpp
extern "C" {
struct Vec3 { int x, y, z; };
extern int MATRIX_SCRATCH_PAPER;
extern void func_ov002_020ed684(void *c);
extern void _ZN9Animation7AdvanceEv(void *c);
extern void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);
extern void MulVec3Mat4x3(void *in, void *mtx, void *out);
extern void Vec3_Add(void *out, void *a, void *b);
extern void _ZN9ModelBase12ApplyOpacityEj(void *self, unsigned int op, int z);
extern void func_ov002_020ed998(void *c);
extern void func_ov002_020ed7f8(void *c);
extern void func_ov002_020edca4(void *c);

int func_ov002_020ee010(char *self)
{
    Vec3 vin;
    Vec3 vmid;
    Vec3 vout;
    func_ov002_020ed684(self);
    _ZN9Animation7AdvanceEv(self + 0x350);
    if (*(int *)(self + 0x3f0) != 1) {
        if (*(unsigned char *)(*(char **)(self + 0x38c) + 0x6f5) < 0xa) {
            short *ang;
            vin.z = 0;
            vin.z = -0x68000;
            vin.x = 0;
            vin.y = 0;
            vmid.x = 0;
            vmid.y = 0;
            vmid.z = 0;
            ang = (short *)(((int)*(char **)(self + 0x38c) + 0x8c) & 0xFFFFFFFFFFFFFFFF);
            Matrix4x3_FromRotationZXYExt(&MATRIX_SCRATCH_PAPER, ang[0], ang[1], ang[2]);
            MulVec3Mat4x3(&vin, &MATRIX_SCRATCH_PAPER, &vmid);
            Vec3_Add(&vout, *(char **)(self + 0x38c) + 0x5c, &vmid);
            *(int *)(self + 0x5c) = vout.x;
            *(int *)(self + 0x60) = vout.y;
            *(int *)(self + 0x64) = vout.z;
        }
        _ZN9ModelBase12ApplyOpacityEj(self + 0x300, *(unsigned char *)(*(char **)(self + 0x38c) + 0x6f5), 0);
    } else {
        _ZN9ModelBase12ApplyOpacityEj(self + 0x300, 0x1f, 0);
    }
    func_ov002_020ed998(self);
    func_ov002_020ed7f8(self);
    {
        unsigned char idx = *(unsigned char *)(self + 0x420);
        if (*(unsigned char *)(self + idx + 0x421) != 0) {
            unsigned char *cp = (unsigned char *)(((int)self + 0x420) & 0xFFFFFFFFFFFFFFFF);
            *cp = *cp + 1;
        }
    }
    if (*(unsigned char *)(self + 0x420) >= 5) {
        func_ov002_020edca4(self);
    }
    return 1;
}
}
