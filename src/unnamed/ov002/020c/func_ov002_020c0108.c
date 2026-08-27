typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

typedef struct { s32 x, y, z; } Vector3;

typedef struct { void *a; u32 b; } PathPtr;

extern void _ZN7PathPtrC1Ev(PathPtr *thiz);
extern void _ZN7PathPtr6FromIDEj(PathPtr *thiz, u32 id);
extern int _ZNK7PathPtr8NumNodesEv(PathPtr *thiz);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(PathPtr *thiz, Vector3 *out, u32 idx);
extern u32 func_0203ad54(unsigned char **c);
extern int Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern int Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
extern int _ZN6Player7IsStateERNS_5StateE(void *self, void *state);

extern char data_ov002_02110304;
extern char data_ov002_0211025c;
extern s32 data_ov002_020ff200[];
extern s16 data_02082214[];

#define AT(p, off) ((void *)(int)((char *)(p) + (off)))

int func_ov002_020c0108(char *self, int p1)
{
    if (*(u8*)(self + 0x6f9) != 0
        || (*(u8*)(self + 0x709) != 0 && p1 != 0)
        || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110304)
        || _ZN6Player7IsStateERNS_5StateE(self, &data_ov002_0211025c))
        return 0;

    if (*(s32*)(self + 0x664) == 0xb)
        return 0;

    if (*(u32*)(self + 0x670) != 0xff) {
        s32 s;
        s32 iX, iY;
        s32 w;
        int idx;
        int numNodes;
        int i;
        int aH, aV;
        Vector3 nodes[3];
        PathPtr path;

        _ZN7PathPtrC1Ev(&path);
        _ZN7PathPtr6FromIDEj(&path, *(u32*)(self + 0x670));
        numNodes = _ZNK7PathPtr8NumNodesEv(&path);

        for (i = 0; i < numNodes; i++) {
            _ZNK7PathPtr7GetNodeER7Vector3j(&path, &nodes[i], i);
        }

        idx = func_0203ad54((unsigned char **)&path) & 0xff;
        if ((u32)idx >= 4)
            idx = 1;

        aH = Vec3_HorzAngle(&nodes[0], &nodes[1]);
        aV = Vec3_VertAngle(&nodes[0], &nodes[1]);

        iY = (u16)aV >> 4;
        iX = (u16)aH >> 4;

        w = data_ov002_020ff200[idx] << 12;

        *(s32 *)AT(self, 0x60) += (s32)(((long long)w * data_02082214[iY * 2] + 0x800) >> 12);
        s = (s32)(((long long)w * data_02082214[iY * 2 + 1] + 0x800) >> 12);
        *(s32 *)AT(self, 0x5c) += (s32)(((long long)s * data_02082214[iX * 2] + 0x800) >> 12);
        *(s32 *)AT(self, 0x64) += (s32)(((long long)s * data_02082214[iX * 2 + 1] + 0x800) >> 12);

        return 1;
    }

    return 0;
}
