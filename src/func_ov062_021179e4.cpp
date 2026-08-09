//cpp
extern "C" {
typedef unsigned int u32;
typedef signed char s8;
typedef int s32;
struct Vector3 { s32 x,y,z; Vector3(s32 X,s32 Y,s32 Z):x(X),y(Y),z(Z){} Vector3(){} };
struct Vector3_16 { short x,y,z; };
struct Actor { char pad1[0x5c]; Vector3 pos; char pad2[0x64]; s8 fcc; };
extern Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(u32,u32,const Vector3&,const Vector3_16*,s32,s32);
void func_ov062_021179e4(Actor* c) {
    Vector3 pos(c->pos.x, c->pos.y + 0x78000, c->pos.z);
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x122, 2, pos, (const Vector3_16*)0, (s32)c->fcc, -1);
}
}
