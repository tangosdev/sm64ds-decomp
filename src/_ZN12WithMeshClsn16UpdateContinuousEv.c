typedef unsigned char u8;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct ClsnResult { char pad[0x28]; } ClsnResult;

extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
extern int func_020355a0(void* p);
extern int _ZNK12WithMeshClsn15ShouldUpdatePosEv(void* p);
extern int func_02037938(void* p);
extern void func_02038324(int a, int* b, int c, int d);
extern void _ZN10ClsnResultC1Ev(ClsnResult* r);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, Vec3* a, Vec3* b, void* actor);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern void _ZN11RaycastLine10GetClsnPosEv(Vec3* out, void* self);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void* self, Vec3* out);
extern int func_02039794(int x);
extern void _ZNK10ClsnResult6CopyToERS_(void* self, ClsnResult* dst);
extern void _ZN12WithMeshClsn19ClearAllGroundFlagsEv(void* p);
extern void _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(void* self, Vec3* v, int rad, void* actor);
extern int func_0203553c(void* p);
extern void _ZN10SphereClsn14SetFloorResultERK10ClsnResult(void* self, ClsnResult* r);
extern void _ZN10ClsnResultaSERKS_(void* self, ClsnResult* r);
extern void func_020371b0(void* clsn, int justHit);
extern void func_02037888(void* dst, ClsnResult* src);
extern int _ZN10SphereClsn10DetectClsnEv(void* self);
extern int _ZNK12WithMeshClsn16ShouldUpdatePosYEv(void* p);
extern void func_020356d4(void* self);
extern void _ZN10ClsnResultD1Ev(ClsnResult* r);

#pragma opt_common_subs off

void _ZN12WithMeshClsn16UpdateContinuousEv(char* c)
{
    int floorFlag;
    int wallFlag;
    int height;
    int onGround;
    int* pos;
    int* prev;
    int handled;
    ClsnResult res0;
    ClsnResult res1;
    Vec3 lineStart, lineEnd;
    Vec3 clsnPos, normal;
    Vec3 newStart, newEnd;
    Vec3 clsnPos2, normal2;
    Vec3 sphere;
    char* a;

    a = *(char**)(c + 0x14);
    pos = (int*)(a + 0x5c);
    prev = (int*)(a + 0x68);

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c) && func_020355a0(c) && _ZNK12WithMeshClsn15ShouldUpdatePosEv(c))
        func_02038324(func_02037938(c + 0x20), pos, *(int*)(c + 0x12c), *(int*)(c + 0x130));

    floorFlag = 0;
    _ZN10ClsnResultC1Ev(&res0);
    wallFlag = 0;
    _ZN10ClsnResultC1Ev(&res1);

    height = *(int*)(c + 0x1c);
    {
        int tx = prev[0];
        int tz = prev[2];
        int ty = prev[1] + height;
        lineStart.x = tx;
        lineStart.y = ty;
        lineStart.z = tz;
    }
    {
        int tx = pos[0];
        int tz = pos[2];
        int ty = pos[1] + height;
        lineEnd.x = tx;
        lineEnd.y = ty;
        lineEnd.z = tz;
    }
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(c + 0x134, &lineStart, &lineEnd, *(void**)(c + 0x14));
    if (_ZN11RaycastLine10DetectClsnEv(c + 0x134))
    {
        int r;
        _ZN11RaycastLine10GetClsnPosEv(&clsnPos, c + 0x134);
        _ZNK11SurfaceInfo12CopyNormalToER7Vector3(c + 0x148, &normal);
        newStart.x = clsnPos.x + (normal.x >> 2);
        newStart.y = (normal.y >> 2) + clsnPos.y;
        newStart.z = clsnPos.z + (normal.z >> 2);
        newEnd.x = newStart.x;
        newEnd.y = newStart.y - height;
        newEnd.z = newStart.z;
        r = func_02039794(normal.y);
        if (r == 1) {
            wallFlag = 1;
            _ZNK10ClsnResult6CopyToERS_(c + 0x144, &res1);
        } else if (r == 0) {
            floorFlag = 1;
            _ZNK10ClsnResult6CopyToERS_(c + 0x144, &res0);
        }
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(c + 0x134, &newStart, &newEnd, *(void**)(c + 0x14));
        if (_ZN11RaycastLine10DetectClsnEv(c + 0x134)) {
            _ZN11RaycastLine10GetClsnPosEv(&clsnPos2, c + 0x134);
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3(c + 0x148, &normal2);
            if (func_02039794(normal2.y) == 0) {
                floorFlag = 1;
                _ZNK10ClsnResult6CopyToERS_(c + 0x144, &res0);
            }
            if (_ZNK12WithMeshClsn15ShouldUpdatePosEv(c)) {
                pos[0] = clsnPos2.x - (normal2.x >> 2);
                pos[1] = clsnPos2.y - (normal2.y >> 2) - (height >> 1);
                pos[2] = clsnPos2.z - (normal2.z >> 2);
            }
        } else if (_ZNK12WithMeshClsn15ShouldUpdatePosEv(c)) {
            pos[0] = newStart.x;
            pos[1] = newStart.y - height;
            pos[2] = newStart.z;
        }
    }

    onGround = _ZNK12WithMeshClsn10IsOnGroundEv(c);
    handled = 0;
    _ZN12WithMeshClsn19ClearAllGroundFlagsEv(c);
    sphere.x = pos[0];
    sphere.y = pos[1];
    sphere.z = pos[2];
    sphere.y += height;
    _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(c + 0x20, &sphere, *(int*)(c + 0x18), *(void**)(c + 0x14));
    if (func_0203553c(c) == 0)
        *(u8*)AT(c, 0x90) |= 0x40;
    *(int*)(c + 0x128) = *(int*)(c + 0x1b8);
    if (pos[1] - prev[1] > 0)
        *(u8*)AT(c, 0x90) |= 0x20;
    if (floorFlag != 0) {
        *(u8*)AT(c, 0x90) |= 4;
        _ZN10SphereClsn14SetFloorResultERK10ClsnResult(c + 0x20, &res0);
        *(u8*)AT(c, 0x90) |= 1;
        _ZN10ClsnResultaSERKS_(c + 0x30, &res0);
        func_020371b0(c, onGround);
        handled = 1;
    }
    if (wallFlag != 0) {
        *(u8*)AT(c, 0x90) |= 8;
        func_02037888(c + 0x20, &res1);
        *(u8*)AT(c, 0x90) |= 1;
        _ZN10ClsnResultaSERKS_(c + 0x30, &res1);
    }
    if (_ZN10SphereClsn10DetectClsnEv(c + 0x20)) {
        prev = (int*)(c + 0x6c);
        if ((*(u8*)(c + 0x90) & 4) && handled == 0)
            func_020371b0(c, onGround);
        if (_ZNK12WithMeshClsn15ShouldUpdatePosEv(c)) {
            pos[0] += prev[0];
            if (_ZNK12WithMeshClsn16ShouldUpdatePosYEv(c))
                *(int*)AT(pos, 4) += prev[1];
            *(int*)AT(pos, 8) += prev[2];
        }
    }
    if (onGround && _ZNK12WithMeshClsn10IsOnGroundEv(c) == 0)
        func_020356d4(c);
    _ZN10ClsnResultD1Ev(&res1);
    _ZN10ClsnResultD1Ev(&res0);
}
