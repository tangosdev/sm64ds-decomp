
#pragma opt_strength_reduction off
#pragma opt_common_subs off
typedef struct Vector3 { int x, y, z; } Vector3;

extern void Vec3_Sub(void *out, void *a, void *b);
extern int NormalizeVec3IfNonZero(void *v);
extern void Vec3_MulScalarInPlace(void *v, int s);
extern short Vec3_HorzAngle(void *a, void *b);
extern void Vec3_Add(void *out, void *a, void *b);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, void *pos, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void _ZN13RaycastGroundD1Ev(void *self);

void func_ov034_02112874(char *sl)
{
    int sb;
    char *r8;
    char *r7;
    char *r6;
    int zero;
    Vector3 diff;
    Vector3 castPos;
    Vector3 sum;
    char ray[0x54];

    r8 = (char *)(sl + 0x3cc) + 0xc;
    r7 = sl + 6;
    r6 = sl + 0xc;
    sb = 1;
    zero = 0;

    for (; sb < 5; sb++, r8 += 0xc, r7 += 6, r6 += 0xc) {
        int off = (sb - 1) * 0xc;
        Vec3_Sub(&diff, r8, (char *)(sl + 0x3cc) + off);
        if (NormalizeVec3IfNonZero(&diff) != 0) {
            Vec3_MulScalarInPlace(&diff, *(int *)(sl + (sb << 2) + 0x464));
        }
        {
            char *prev = (char *)(sl + 0x3cc) + off;
            short ang = Vec3_HorzAngle(r8, prev);
            *(short *)(r7 + 0x446) = ang;
            Vec3_Add(&sum, prev, &diff);
        }

        *(int *)(r6 + 0x3cc) = sum.x;
        *(int *)(r6 + 0x3d0) = sum.y;
        *(int *)(r6 + 0x3d4) = sum.z;
        *(int *)(sl + (sb << 6) + 0x4ac) = *(int *)(r6 + 0x3cc);
        *(int *)(sl + (sb << 6) + 0x4b0) = *(int *)(r6 + 0x3d0);
        *(int *)(sl + (sb << 6) + 0x4b4) = *(int *)(r6 + 0x3d4);
        *(int *)(sl + (sb << 6) + 0x5ec) = *(int *)(r6 + 0x3cc);
        *(int *)(sl + (sb << 6) + 0x5f0) = *(int *)(r6 + 0x3d0);
        *(int *)(sl + (sb << 6) + 0x5f4) = *(int *)(r6 + 0x3d4);

        if (*(unsigned char *)(sl + 0x8df) == 0) {
            int py;
            _ZN13RaycastGroundC1Ev(ray);
            castPos.x = *(int *)(r6 + 0x3cc);
            py = *(int *)(r6 + 0x3d0);
            castPos.y = py;
            castPos.z = *(int *)(r6 + 0x3d4);
            castPos.y = py + 0x3c000;
            _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(ray, &castPos, (void *)zero);
            if (_ZN13RaycastGround10DetectClsnEv(ray) != 0) {
                int clY = *(int *)(ray + 0x44);
                if (*(int *)(r6 + 0x3d0) <= clY)
                    *(int *)(r6 + 0x3d0) = clY;
            }
            _ZN13RaycastGroundD1Ev(ray);
        }
    }
}
