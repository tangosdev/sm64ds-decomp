//cpp
typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
struct Vector3 { s32 x, y, z; };
struct Vector3_16;
struct Actor;
extern "C" Actor *_ZN5Actor13ClosestPlayerEv(void);
extern "C" s32 Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern "C" Actor *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32,u32,const Vector3*,const Vector3_16*,s32,s32);
extern "C" void func_0201267c(u32 a, void *b, void *c, int d);
extern s16 data_02082214[];
#define M(x) ((long long)(int)(x) & 0xFFFFFFFFFFFFFFFFLL)

extern "C" int func_ov098_0213b9d8(char *self)
{
    Vector3 v; Actor *player; s32 ang; s32 vel; u32 ua; int factor; int a; s16 sn, cs;
    int nx, ny, nz; int py, sy;
    if (*(u16 *)(self + 0x100)) return;
    player = _ZN5Actor13ClosestPlayerEv();
    if (!player) return;
    { char *p = (char *)(int)M((char *)player + 0x5c);
      v.x=*(int*)p; v.y=*(int*)(p+4); v.z=*(int*)(p+8); }
    if (Vec3_HorzDist(&v, (Vector3 *)(self + 0x5c)) > 0x5dc000) return;
    py = v.y;
    sy = *(int *)(self + 0x60);
    if (py > sy) return sy;
    ang = *(s16 *)((char *)player + 0x94);
    vel = *(int *)((char *)player + 0x98);
    ny = py + 0x480000;
    ua = (u32)(ang << 16) >> 16;
    a = (int)ua >> 4;
    factor = vel >> 0xc;
    factor = factor * 0x1c;
    cs = data_02082214[a * 2];
    sn = data_02082214[a * 2 + 1];
    factor = factor + 0x64;
    nx = factor * cs + v.x;
    v.y = ny;
    v.x = nx;
    nz = factor * sn + v.z;
    v.z = nz;
    { Actor *spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xd0,1,&v,0,*(signed char*)(self+0xcc),-1);
      *(int*)((char*)spawned+0x98)=0; }
    *(u16*)(self+0x100)=0x96;
    func_0201267c(0xd8, self+0x74, self+0x100, 0x96);
}
