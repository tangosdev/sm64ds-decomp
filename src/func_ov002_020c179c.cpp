//cpp
// func_ov002_020c179c at 0x020c179c
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
struct Vector3 { int x, y, z; };
extern "C" {
extern void _ZN13RaycastGroundC1Ev(void* self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void* self, void* pos, void* act);
extern int _ZN13RaycastGround10DetectClsnEv(void* self);
extern void _ZN13RaycastGroundD1Ev(void* self);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern short SINE_TABLE[];

int func_ov002_020c179c(char* self, int angle) {
    char rg[0x50];
    Vector3 v1;
    Vector3 v2;
    int idx = (unsigned short)(short)(*(short*)(self + 0x94) + angle);
    int y1 = *(int*)(self + 0x60);
    int y2 = y1;
    idx >>= 4;
    short dx = SINE_TABLE[idx * 2];
    short dy = SINE_TABLE[idx * 2 + 1];
    int ox = dx * 5;
    int oy = dy * 5;
    _ZN13RaycastGroundC1Ev(rg);
    {
        int z = *(int*)(self + 0x64) + oy;
        int y = *(int*)(self + 0x60) + 0x64000;
        int x = *(int*)(self + 0x5c) + ox;
        v1.x = x;
        v1.y = y;
        v1.z = z;
    }
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, &v1, self);
    if (_ZN13RaycastGround10DetectClsnEv(rg)) y1 = *(int*)(rg + 0x44);
    {
        int z = *(int*)(self + 0x64) - oy;
        int x = *(int*)(self + 0x5c) - ox;
        int y = *(int*)(self + 0x60) + 0x64000;
        v2.x = x;
        v2.y = y;
        v2.z = z;
    }
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, &v2, self);
    if (_ZN13RaycastGround10DetectClsnEv(rg)) y2 = *(int*)(rg + 0x44);
    {
        int r = _ZN4cstd5atan2E5Fix12IiES1_(y1 - y2, 0xa000);
        _ZN13RaycastGroundD1Ev(rg);
        return r;
    }
}
}
