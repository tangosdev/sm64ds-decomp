//cpp
typedef unsigned int u32;
typedef short s16;
typedef int Fix12i;

struct Vec3 { Fix12i x, y, z; };
struct Mtx43 { Fix12i a[12]; };
struct RaycastGround { char buf[0x68 - 0x18]; };

extern "C" {
void Vec3_Asr(struct Vec3 *d, struct Vec3 *s, int sh);
void Matrix4x3_FromTranslation(struct Mtx43 *m, Fix12i x, Fix12i y, Fix12i z);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 ang);
void Matrix4x3_ApplyInPlaceToRotationZ(void *m, s16 ang);
void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 ang);
void Matrix4x3_FromRotationY(void *m, int angle);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *mtx, Fix12i a, Fix12i b, u32 u);
void _ZN13RaycastGroundC1Ev(struct RaycastGround *self);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RaycastGround *self, const struct Vec3 *v, void *actor);
int _ZN13RaycastGround10DetectClsnEv(struct RaycastGround *self);
void _ZN13RaycastGroundD1Ev(struct RaycastGround *self);
}

extern struct Mtx43 MATRIX_SCRATCH_PAPER;
extern unsigned char CURRENT_GAMEMODE;

extern "C" void func_ov002_020b2c44(char *c)
{
    struct RaycastGround rg;
    struct Vec3 pos;
    struct Vec3 v;
    int b;

    Vec3_Asr(&v, (struct Vec3 *)(c + 0x5c), 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, *(s16 *)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationZ(&MATRIX_SCRATCH_PAPER, *(s16 *)(c + 0x90));
    Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, *(s16 *)(c + 0x8c));
    *(struct Mtx43 *)(c + 0xf0) = MATRIX_SCRATCH_PAPER;

    b = (CURRENT_GAMEMODE == 1);
    if (b) {
        Matrix4x3_FromRotationY(c + 0x33c, *(s16 *)(c + 0x8e));
        *(int *)(c + 0x360) = *(int *)(c + 0x5c) >> 3;
        *(int *)(c + 0x364) = *(int *)(c + 0x60) >> 3;
        *(int *)(c + 0x368) = *(int *)(c + 0x64) >> 3;
        _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x314, c + 0x33c, 0x96000, 0x320000, 0xf);
        return;
    }

    {
        int r5;
        int r4;

        _ZN13RaycastGroundC1Ev(&rg);
        pos.x = *(int *)(c + 0x5c);
        pos.y = *(int *)(c + 0x60);
        pos.z = *(int *)(c + 0x64);
        pos.y = pos.y + 0x14000;
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &pos, c);
        _ZN13RaycastGround10DetectClsnEv(&rg);

        r5 = *(int *)(c + 0x60) - *(int *)((char *)&rg + 0x44);
        if (r5 <= 0x1000)
            r5 = 0x1000;
        r4 = 0x96000 - (int)(((long long)r5 * 0x180 + 0x800) >> 12);
        if (r4 < 0xa000)
            r4 = 0xa000;

        Matrix4x3_FromRotationY(c + 0x33c, *(s16 *)(c + 0x8e));
        *(int *)(c + 0x360) = *(int *)(c + 0x5c) >> 3;
        *(int *)(c + 0x364) = *(int *)(c + 0x60) >> 3;
        *(int *)(c + 0x368) = *(int *)(c + 0x64) >> 3;
        _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x314, c + 0x33c, r4, r5 + 0x28000, 0xf);
        _ZN13RaycastGroundD1Ev(&rg);
    }
}
