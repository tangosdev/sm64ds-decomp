typedef short s16;
typedef struct { int m[12]; } Matrix4x3;
typedef struct { int x, y, z; } Vector3;

extern Matrix4x3 data_020a0e68;

extern int _ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(void *c, s16 a, s16 b, s16 d, int fix);
extern void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle);
extern void MulVec3Mat4x3(Vector3 *v, Matrix4x3 *m, Vector3 *dst);
extern void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern void _ZN13RaycastGroundC1Ev(char *thiz);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(char *thiz, Vector3 *v, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(char *thiz);
extern void _ZN13RaycastGroundD1Ev(char *thiz);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *thiz, void *clsn);
extern void func_020383fc(void *p);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *thiz);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *thiz);
extern void func_020393a4(int *p, int v);
extern void func_02039394(int *p, int v);
extern int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void *thiz, int a, int b);
extern int Vec3_Dist(Vector3 *a, Vector3 *b);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, Vector3 *pos, unsigned int e);
extern int Vec3_HorzDist(Vector3 *a, Vector3 *b);
extern void func_ov002_020f0438(void *thiz);

int func_ov002_020b8dd4(char *c)
{
    Vector3 v;
    Vector3 dst;
    volatile Vector3 pos;
    char ray[0x50];
    char *q;

    if (_ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(c, 0x1800, 0, 0, 0x96000) != 0) {
        return 1;
    }

    if (*(int *)(c + 0x98) != 0) {
        v.x = 0;
        dst.x = 0;
        dst.y = 0;
        dst.z = 0;
        v.y = 0x96000;
        v.z = 0x96000;
        Matrix4x3_FromRotationY(&data_020a0e68, *(s16 *)(c + 0x94));
        MulVec3Mat4x3(&v, &data_020a0e68, &dst);
        AddVec3(&dst, (Vector3 *)(c + 0x5c), &dst);
        _ZN13RaycastGroundC1Ev(ray);
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(ray, &dst, 0);
        if (_ZN13RaycastGround10DetectClsnEv(ray) != 0) {
            if (*(int *)(c + 0x4e8) != *(int *)(ray + 0x44)) {
                *(int *)(c + 0x98) = 0;
            }
        } else {
            *(int *)(c + 0x98) = 0;
        }
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
        func_020383fc(c + 0x320);
        _ZN8Platform21UpdateModelPosAndRotYEv(c);
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
        _ZN13RaycastGroundD1Ev(ray);
    }

    func_020393a4((int *)(c + 0x124), 0x12c000);
    func_02039394((int *)(c + 0x124), 0x96000);
    _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0x3e8000, 0x96000);

    if (*(int *)(c + 0x98) != 0 && Vec3_Dist((Vector3 *)(c + 0x5c), (Vector3 *)(c + 0x68)) != 0) {
        *(unsigned int *)(c + 0x4ec) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int *)(c + 0x4ec), 3, 0x97, (Vector3 *)(c + 0x74), 0);
    }

    if (Vec3_HorzDist((Vector3 *)(c + 0x4dc), (Vector3 *)(c + 0x5c)) >= 0x12c000) {
        if (*(int *)(c + 0x4f0) != 0) {
            if (*(unsigned short *)(*(int *)(c + 0x4f0) + 0xc) == 0x149) {
                pos.x = *(int *)(c + 0x4dc);
                pos.y = *(int *)(c + 0x4e0);
                pos.z = *(int *)(c + 0x4e4);
                pos.y = *(int *)(c + 0x4e0) + 0x96000;
                q = *(char **)(c + 0x4f0);
                *(int *)(q + 0x5c) = *(int *)(c + 0x4dc);
                *(int *)(q + 0x60) = pos.y;
                *(int *)(q + 0x64) = pos.z;
                func_ov002_020f0438(*(void **)(c + 0x4f0));
            }
            *(void **)(c + 0x4f0) = 0;
        }
    }

    *(int *)(c + 0x98) = 0;
    return 1;
}
