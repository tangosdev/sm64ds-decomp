typedef unsigned char u8;
typedef unsigned short u16;

enum { false, true };

typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct BF { u8 f0 : 2; u8 f1 : 3; u8 f2 : 3; } BF;

extern int data_ov002_020ff078[];

void func_ov002_020b13e0(char *c);
void func_ov002_020b1384(char *c);
char *_ZN5Actor10FindWithIDEj(int id);
void _ZN5Sound9PlayBank3EjRK7Vector3(int a, void *v);
int _ZNK12WithMeshClsn10IsOnGroundEv(char *w);
char *_ZNK12WithMeshClsn14GetFloorResultEv(char *w);
int func_02037e58(char *s);
void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(char *s, Vector3 *v);
int _ZNK12WithMeshClsn13JustHitGroundEv(char *w);
int SurfaceInfo_TestFlag0x20(char *s);
int Vec3_HorzLen(void *v);
void _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(char *c);
int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
int _ZN4cstd4fdivEii(int a, int b);

void func_ov002_020b1cc0(char *c)
{
    Vector3 n;
    char *fl;
    int attr;
    int t;
    int v;
    int r3v;
    BF *bf;
    char *p;

    func_ov002_020b13e0(c);
    func_ov002_020b1384(c);
    p = _ZN5Actor10FindWithIDEj(*(int *)(c + 0xd4));
    if (p != 0) {
        t = *(u16 *)(p + 0xc);
        t = t == 0x4f;
        if (t != false) {
            if (*(int *)(c + 0x60) <= *(int *)(p + 0x60)) {
                *(int *)(c + 0x60) = *(int *)(p + 0x60);
                _ZN5Sound9PlayBank3EjRK7Vector3(0x52, c + 0x74);
                bf = (BF *)(((int)c + 0x3ae));
                bf->f1++;
                v = ((BF *)(c + 0x3ae))->f1;
                if (v < 3u)
                    r3v = ((3 - v) << 12) / 3;
                else
                    r3v = 0x555;
                *(int *)(c + 0xa8) = -*(int *)(c + 0xa8) * r3v / 0x1000;
                *(int *)(((int)c + 0x98)) >>= 1;
                if (*(int *)(c + 0xa8) >= -*(int *)(c + 0x9c))
                    return;
                *(int *)(c + 0xa8) = 0;
                *(int *)(c + 0x98) = 0;
                *(int *)(c + 0x9c) = 0;
                *(int *)(c + 0x3a4) = 4;
                return;
            }
        }
    }

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x1ac) == 0)
        return;

    fl = _ZNK12WithMeshClsn14GetFloorResultEv(c + 0x1ac);
    attr = func_02037e58(fl + 4);
    _ZNK11SurfaceInfo12CopyNormalToER7Vector3(fl + 4, &n);

    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x1ac) != 0) {
        if (SurfaceInfo_TestFlag0x20(_ZNK12WithMeshClsn14GetFloorResultEv(c + 0x1ac) + 4) == 0) {
            if (*(u16 *)(c + 0x3a8) > 0xf000)
                *(u16 *)(c + 0x3a8) = 0xf;
            _ZN5Sound9PlayBank3EjRK7Vector3(0x52, c + 0x74);
            bf = (BF *)(((int)c + 0x3ae));
            bf->f1++;
            v = ((BF *)(c + 0x3ae))->f1;
            if (v < 3u)
                r3v = ((3 - v) << 12) / 3;
            else
                r3v = 0x555;
            *(int *)(c + 0xa8) = -*(int *)(c + 0xa8) * r3v / 0x1000;
            *(int *)(((int)c + 0x98)) >>= 1;
        }
    }

    if (n.x != 0 || n.z != 0) {
        int m6 = data_ov002_020ff078[attr];
        int *pa = (int *)(((int)c + 0xa4));
        *pa += n.x * m6;
        *(int *)(((int)c + 0xac)) += n.z * m6;
        *(int *)(c + 0x98) = Vec3_HorzLen(pa);
        if (*(int *)(c + 0x98) > 0x1c000) {
            *(int *)(c + 0x98) = 0x1c000;
            _ZN5Actor28UpdatePosWithHorzSpeedAndAngEv(c);
        }
        *(u16 *)(c + 0x94) = _ZN4cstd5atan2E5Fix12IiES1_(*(int *)(c + 0xa4), *(int *)(c + 0xac));
    }

    if (_ZNK12WithMeshClsn13JustHitGroundEv(c + 0x1ac) != 0)
        return;
    if (SurfaceInfo_TestFlag0x20(_ZNK12WithMeshClsn14GetFloorResultEv(c + 0x1ac) + 4) != 0)
        return;

    if (n.y != 0) {
        int v1 = (int)(((long long)n.x * *(int *)(c + 0xa4) + 0x800) >> 12);
        int v2 = (int)(((long long)n.z * *(int *)(c + 0xac) + 0x800) >> 12);
        *(int *)(c + 0xa8) = -(_ZN4cstd4fdivEii(v1 + v2, n.y) + 0x8000);
    }

    if ((unsigned)(attr - 1) <= 1) {
        *(int *)(c + 0x98) = 0;
    } else {
        *(int *)(c + 0x98) = *(int *)(c + 0x98) * 0xc00 / 0x1000;
    }

    if (*(int *)(c + 0x98) >= 0x1000)
        return;
    *(int *)(c + 0xa8) = 0;
    *(int *)(c + 0x98) = 0;
    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0x3a4) = 4;
}
