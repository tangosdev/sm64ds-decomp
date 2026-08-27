typedef short s16;
typedef long long s64;

typedef struct { int x, y, z; } Vector3;

extern void _Z11UpdateAngleRssis(short *cur, short target, int rate, short delta);
extern int _Z14ApproachLinearRiii(int *cur, int target, int step);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern int NormalizeVec3IfNonZero(Vector3 *v);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern void func_ov006_020c057c(char *c);
extern int _Z15ApproachLinear2Rsss(short *cur, short target, short step);
extern int _ZNK9Animation12WillHitFrameEi(char *anim, int frame);
extern void func_ov006_020c0304(char *c);

void func_ov006_020c0364(char *c)
{
    char *path;
    char *arr;
    int idx;
    int a, b;
    int t1, t2;
    int r1, r2;

    _Z11UpdateAngleRssis((short *)(c + 0xea), *(short *)(c + 0xee), 8, 0x100);

    a = *(int *)(c + 0xd4);
    if (a < 0)
        a = -a;
    t1 = (int)(((s64)a * 0x1a9 + 0x800) >> 12);

    b = *(int *)(c + 0xdc);
    if (b < 0)
        b = -b;

    idx = *(int *)(c + 0xe0) * 0xc;
    path = *(char **)(c + 0xb0);
    arr = *(char **)(path + 4);
    r1 = _Z14ApproachLinearRiii((int *)(c + 0xc8), *(int *)(arr + idx), t1);

    t2 = (int)(((s64)b * 0x1a9 + 0x800) >> 12);

    idx = *(int *)(c + 0xe0) * 0xc;
    path = *(char **)(c + 0xb0);
    arr = *(char **)(path + 4);
    r2 = _Z14ApproachLinearRiii((int *)(c + 0xd0), *(int *)(arr + idx + 8), t2);

    if (r1 != 0 && r2 != 0) {
        if (*(int *)(c + 0xe0) < *(int *)(*(char **)(c + 0xb0)) - 1) {
            Vector3 dir;
            Vector3 next;
            int *p;

            p = (int *)(c + 0xe0);
            *p = *p + 1;

            path = *(char **)(c + 0xb0);
            idx = *(int *)(c + 0xe0) * 0xc;
            arr = *(char **)(path + 4);
            Vec3_Sub(&dir, (Vector3 *)(c + 0xc8), (Vector3 *)(arr + idx));

            *(int *)(c + 0xd4) = dir.x;
            *(int *)(c + 0xd8) = dir.y;
            *(int *)(c + 0xdc) = dir.z;

            if (NormalizeVec3IfNonZero((Vector3 *)(c + 0xd4)) == 0) {
                *(int *)(c + 0xd4) = 0xb50;
                *(int *)(c + 0xd8) = 0xb50;
                *(int *)(c + 0xdc) = 0;
            }

            idx = *(int *)(c + 0xe0) * 0xc;
            path = *(char **)(c + 0xb0);
            arr = *(char **)(path + 4);
            next.x = *(int *)(arr + idx);
            next.y = *(int *)(arr + idx + 4);
            next.z = *(int *)(arr + idx + 8);

            *(s16 *)(c + 0xee) = Vec3_HorzAngle((const Vector3 *)(c + 0xc8), &next);
            return;
        }

        func_ov006_020c057c(c);
        return;
    }

    if (_Z15ApproachLinear2Rsss((short *)(c + 0xf2), 0, 1) == 0)
        return;

    if (_ZNK9Animation12WillHitFrameEi(c + 0x68, 0) == 0)
        return;

    func_ov006_020c0304(c);
}
