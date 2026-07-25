typedef unsigned char u8;
typedef short s16;

typedef struct { int w[12]; } M48;

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *bmd, int a, int b);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thiz, void *kcl, void *mtx, int fix, short s, void *clps);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void *thiz, void *actor);

int func_ov002_020b4d58(char *c, char *arg)
{
    int i;
    char *pp;
    char *mb;
    int j;
    char *mtx;
    int fix = 0x1000;
    char *dst;
    char *cl;

    pp = arg;
    mb = c + 0x320;
    for (i = 0; i < 5; i++) {
        _ZN9ModelBase7SetFileEP8BMD_Fileii(mb,
            _ZN5Model8LoadFileER13SharedFilePtr(*(void **)pp), 1, -1);
        pp += 0xc;
        mb += 0x50;
    }

    *(u8 *)(c + 0xdc8) = 4;
    *(u8 *)(c + 0xdc9) = 2;
    *(u8 *)(c + 0xdcb) = *(int *)(c + 8) & 0xf;
    Matrix4x3_FromRotationY(c + 0x33c, *(s16 *)(c + 0x8e));
    *(int *)(c + 0x360) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x364) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x368) = *(int *)(c + 0x64) >> 3;
    *(M48 *)(c + 0xd98) = *(M48 *)(c + 0x33c);
    *(int *)(c + 0xdbc) = *(int *)(c + 0x5c);
    *(int *)(c + 0xdc0) = *(int *)(c + 0x60);
    *(int *)(c + 0xdc4) = *(int *)(c + 0x64);

    j = 0;
    mtx = c + 0x33c;
    dst = c + 0x320;
    cl = c + 0x4b0;
    for (; j < 5; j++) {
        *(M48 *)(dst + 0x1c) = *(M48 *)mtx;
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            cl, _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(void **)(arg + 4)),
            c + 0xd98, fix, *(s16 *)(c + 0x8e), *(void **)(arg + 8));
        if (j == *(u8 *)(c + 0xdc8))
            _ZN16MeshColliderBase6EnableEP5Actor(cl, c);
        dst += 0x50;
        arg += 0xc;
        cl += 0x1c8;
    }
    return 1;
}
