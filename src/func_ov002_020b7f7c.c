/* func_ov002_020b7f7c @ 0x020b7f7c (ov002, size 0x264)
 * Player render-matrix update: syncs render angles outside fixed-angle
 * states, uses the override matrix at +0xc8 if set, otherwise builds
 * translation*rotation into the shared matrix and copies it in; then for
 * grounded states computes the drop shadow (raycast below when airborne).
 */
typedef short s16;
typedef struct {
    int m[12];
} Matrix4x3;

extern Matrix4x3 data_020a0e68;
extern char data_ov002_0210df54;
extern char data_ov002_0210df74;
extern char data_ov002_0210df44;

extern void _ZN9ModelBase12ApplyOpacityEj(char *, int, int);
extern void Vec3_Asr(int *, char *, int);
extern void Matrix4x3_FromTranslation(Matrix4x3 *, int, int, int);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3 *, int, int, int);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(char *);
extern void _ZN13RaycastGroundC1Ev(char *);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(char *, int *, int);
extern int _ZN13RaycastGround10DetectClsnEv(char *);
extern void _ZN13RaycastGroundD1Ev(char *);
extern void Matrix4x3_FromRotationY(char *, int);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(char *, char *, char *, int, int, int);

void func_ov002_020b7f7c(char *c)
{
    int v2[3];
    int v[3];
    char ray[0x50];
    int m = *(int *)(c + 0x3f0);
    if (m != 5 && m != 0x12 && m != 0x10 && m != 0xb && m != 7 && m != 9 &&
        m != 0x14 && m != 0x15 && m != 0x16) {
        *(s16 *)(c + 0x8c) = *(s16 *)(c + 0x92);
        *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
        *(s16 *)(c + 0x90) = *(s16 *)(c + 0x96);
    }

    if (*(Matrix4x3 **)(c + 0xc8) != 0) {
        *(Matrix4x3 *)(c + 0x31c) = **(Matrix4x3 **)(c + 0xc8);
        return;
    }

    if ((unsigned int)(*(int *)(c + 0x3f0) - 6) <= 1)
        _ZN9ModelBase12ApplyOpacityEj(c + 0x300, 0, 0);

    Vec3_Asr(v, c + 0x5c, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v[0], v[1], v[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, *(s16 *)(c + 0x8c),
                                           *(s16 *)(c + 0x8e), *(s16 *)(c + 0x90));
    *(Matrix4x3 *)(c + 0x31c) = data_020a0e68;

    {
        char *s = *(char **)(c + 0x3bc);
        if (s == &data_ov002_0210df54)
            return;
        if (s == &data_ov002_0210df74)
            return;
        if (s == &data_ov002_0210df44)
            return;
    }

    if (*(int *)(c + 0x80) < 0x500)
        return;

    {
        int y = *(int *)(c + 0x60);
        int off;
        if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x144) == 0) {
            v2[0] = *(int *)(c + 0x5c);
            v2[1] = *(int *)(c + 0x60);
            v2[2] = *(int *)(c + 0x64);
            v2[1] = v2[1] + 0x28000;
            _ZN13RaycastGroundC1Ev(ray);
            _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(ray, v2, 0);
            y = v2[1];
            if (_ZN13RaycastGround10DetectClsnEv(ray) != 0)
                y = *(int *)(ray + 0x44);
            _ZN13RaycastGroundD1Ev(ray);
        }

        off = 0;
        {
            int t = *(int *)(c + 0x3f4);
            if (t == 0)
                goto neg;
            if (t == 1) {
            neg:
                off = -10;
            }
        }
        Matrix4x3_FromRotationY(c + 0x38c, *(s16 *)(c + 0x8e));
        *(int *)(c + 0x3b0) = *(int *)(c + 0x5c) >> 3;
        *(int *)(c + 0x3b4) = (y + (off << 12)) >> 3;
        *(int *)(c + 0x3b8) = *(int *)(c + 0x64) >> 3;
        _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x364, c + 0x38c, 0x50000, 0x32000, 0xf);
    }
}
