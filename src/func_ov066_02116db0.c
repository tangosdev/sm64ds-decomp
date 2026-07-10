typedef struct Vec3 { int x, y, z; } Vec3;
typedef short s16;

extern void * _ZN5Actor13ClosestPlayerEv(void *thiz);
extern void func_02012694(int a, void *p);
extern void func_ov066_021166c8(void *thiz);
extern s16 Vec3_HorzAngle(const void *a, const void *b);
extern void Matrix4x3_FromRotationY(void *m, s16 ang);
extern void MulVec3Mat4x3(void *a, void *m, void *b);
extern int ApproachAngle(s16 *angle, int target, int a, int b, int max);
extern int func_ov066_0211603c(void *c);
extern int Vec3_ApproachHorz(void *out, void *a, int maxStep);
extern int func_ov066_02116b78(void *c);
extern int Vec3_HorzDist(const void *a, const void *b);
extern void func_ov066_021165cc(void *c);
extern void func_ov066_021164ec(void *c);
extern int _ZN9Animation8FinishedEv(void *thiz);
extern void func_ov066_021162e8(void *c);
extern int _ZN16MeshColliderBase9IsEnabledEv(void *thiz);
extern void _ZN16MeshColliderBase7DisableEv(void *thiz);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *thiz, void *f, void *m, int fx, int s, void *clps);
extern void func_020393d4(void *p, void *v);
extern void func_020393c4(void *p, void *v);
extern void func_020398fc(void *p);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void *thiz, void *a);
extern void func_ov066_02119454(void *c, void *p);

extern int data_020a0e68[];
extern unsigned char data_ov066_0211ae08;
extern unsigned char data_ov066_0211ae04;
extern char data_ov066_0211ae14[];
extern char data_ov066_0211aeac[];
extern char data_ov066_0211b06c;
extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern int func_02112c08;
extern int func_02112d48;
extern int func_ov066_0211a35c;

int func_ov066_02116db0(void *thiz)
{
    char *c = (char *)thiz;
    Vec3 in, out;
    s16 ang;
    int r;

    switch (*(int *)(c + 0x4a0)) {
    case 0:
        func_ov066_021166c8(c);
        *(int *)(c + 0x4a0) = 1;
        break;

    case 1:
        if (*(int *)(c + 0x498) == 0) {
            char *p = (char *)_ZN5Actor13ClosestPlayerEv(c);
            if (p != 0) {
                Vec3 *pp = (Vec3 *)(((int)p + 0x5c) & 0xFFFFFFFFFFFFFFFF);
                *(int *)(c + 0x4bc) = pp->x;
                *(int *)(c + 0x4c0) = pp->y;
                *(int *)(c + 0x4c4) = pp->z;

                in.x = 0;
                in.y = 0;
                in.z = 0;
                out.x = 0;
                out.y = 0;
                out.z = 0;
                in.z = 0x3e8000;

                ang = Vec3_HorzAngle(c + 0x5c, c + 0x4bc);
                Matrix4x3_FromRotationY(data_020a0e68, ang);
                MulVec3Mat4x3(&in, data_020a0e68, &out);

                *(int *)(((int)c + 0x4bc) & 0xFFFFFFFFFFFFFFFF) += out.x;
                *(int *)(((int)c + 0x4c4) & 0xFFFFFFFFFFFFFFFF) += out.z;
            }

            if ((unsigned short)(*(int *)(c + 0x3b8) >> 0xc) == 0)
                func_02012694(0x140, c + 0x74);

            if ((unsigned short)(*(int *)(c + 0x3b8) >> 0xc) == 0) {
                func_ov066_021164ec(c);
                func_02012694(0x144, c + 0x74);
            }

            ang = Vec3_HorzAngle(c + 0x5c, c + 0x4bc);
            ApproachAngle((s16 *)(c + 0x8e), ang, 2, 0x400, 0x200);
        }

        if (*(int *)(c + 0x498) == 1) {
            r = func_ov066_0211603c(c);
            if (r != 0) {
                if (r == 1)
                    *(int *)(c + 0x4a0) = 2;
                break;
            }
            Vec3_ApproachHorz(c + 0x5c, c + 0x4bc, 0x37000);
            if (func_ov066_02116b78(c) == 1 || Vec3_HorzDist(c + 0x5c, c + 0x4bc) <= 0x37000) {
                func_ov066_021165cc(c);
                *(int *)(c + 0x4a0) = 3;
            }
        }
        break;

    /* case 3 body before case 2 to match ROM placement */
    case 3:
        if (_ZN9Animation8FinishedEv(c + 0x3b0) != 0) {
            if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x674) != 0)
                _ZN16MeshColliderBase7DisableEv(c + 0x674);
            if (*(int *)(c + 0x49c) == 1)
                _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                    c + 0x674, *(void **)(data_ov066_0211ae14 + 4), c + 0x83c, 0x199,
                    *(s16 *)(c + 0x8e), &func_02112c08);
            else
                _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                    c + 0x674, *(void **)(data_ov066_0211aeac + 4), c + 0x83c, 0x199,
                    *(s16 *)(c + 0x8e), &func_02112d48);
            func_020393d4(c + 0x674, &_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
            func_020393c4(c + 0x674, &func_ov066_0211a35c);
            func_020398fc(c + 0x674);
            _ZN16MeshColliderBase6EnableEP5Actor(c + 0x674, c);
            *(int *)(c + 0x4a0) = 4;
        }
        break;

    case 2:
        if (_ZN9Animation8FinishedEv(c + 0x3b0) != 0) {
            func_ov066_021165cc(c);
            *(int *)(c + 0x4a0) = 3;
        }
        break;

    case 4:
        func_ov066_021162e8(c);
        ApproachAngle((s16 *)(c + 0x8e), 0, 2, 0x400, 0x200);
        Vec3_ApproachHorz(c + 0x5c, c + 0x4a4, 0x37000);
        if (Vec3_HorzDist(c + 0x5c, c + 0x4a4) <= 0x37000) {
            *(int *)(c + 0x5c) = *(int *)(c + 0x4a4);
            *(int *)(c + 0x60) = *(int *)(c + 0x4a8);
            *(int *)(c + 0x64) = *(int *)(c + 0x4ac);
            data_ov066_0211ae08 = 2;
            *(s16 *)(c + 0x8e) = 0;
            *(int *)(c + 0x4a0) = 5;
        }
        break;

    case 5:
        if (data_ov066_0211ae04 != 9)
            func_ov066_02119454(c, &data_ov066_0211b06c);
        break;
    }
    return 1;
}
