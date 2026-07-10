//cpp
#pragma opt_strength_reduction off
extern int data_ov034_02114488;
extern char data_020a0e68[];
struct Vec3 { int x, y, z; };
extern "C" int _ZN5Actor18HorzAngleToCPlayerEv(void *self);
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void MulVec3Mat4x3(struct Vec3 *in, void *m, struct Vec3 *out);
extern "C" void Vec3_Add(struct Vec3 *out, struct Vec3 *a, struct Vec3 *b);

extern "C" void func_ov034_02112484(char *sl)
{
    int sb;
    char *r8;
    char *r7;
    int zero;
    struct Vec3 in;
    struct Vec3 out;
    struct Vec3 sum;

    sb = 0;
    *(int *)(sl + 0x8c8) = sb;
    *(int *)(sl + 0x98) = data_ov034_02114488;
    r8 = sl;
    r7 = sl;
    zero = sb;
    for (; sb < 5; sb++, r8 += 0xc, r7 += 6) {
        if (sb == 0) {
            *(int *)(r8 + 0x3cc) = *(int *)(sl + 0x5c);
            *(int *)(r8 + 0x3d0) = *(int *)(sl + 0x60);
            *(int *)(r8 + 0x3d4) = *(int *)(sl + 0x64);
            {
                short ang = (short)_ZN5Actor18HorzAngleToCPlayerEv(sl);
                *(short *)(r7 + 0x446) = ang;
                *(short *)(sl + 0x94) = *(short *)(r7 + 0x446);
            }
        } else {
            int r6;
            int zval;
            char *angbase;
            zval = -*(int *)(sl + (sb << 2) + 0x464);
            angbase = sl + 0x400;
            in.x = zero;
            in.y = zero;
            out.x = zero;
            out.y = zero;
            out.z = zero;
            in.z = zval;
            {
                short ang = *(short *)(angbase + 0x46);
                Matrix4x3_FromRotationY(data_020a0e68, (short)(ang + 0x200));
            }
            MulVec3Mat4x3(&in, data_020a0e68, &out);
            r6 = sb - 1;
            Vec3_Add(&sum, (struct Vec3 *)(sl + 0x3cc + r6 * 0xc), &out);
            *(int *)(r8 + 0x3cc) = sum.x;
            *(int *)(r8 + 0x3d0) = sum.y;
            *(int *)(r8 + 0x3d4) = sum.z;
            *(short *)(r7 + 0x446) = *(short *)(sl + r6 * 6 + 0x446);
        }
    }
}
