/* HOST COPY of src/func_ov098_0213b9d8.cpp -- the WATER_BOMB's state-0 body
 * (the fire state that spawns a bomb toward the nearest player).
 *
 * src/func_ov098_0213b9d8.cpp is byte-matched with mwccarm, and it is declared
 * int-returning while its body uses mwccarm's bare `return;` on every early
 * exit and falls off the end. mwccarm accepts that (r0 carries whatever); MSVC's
 * C++ front end rejects it (C2561, "'func_ov098_0213b9d8': function must return
 * a value"), so the matched TU cannot be compiled into the port as-is and was
 * never in the link -- the state-table seat pointed at unbuilt DS memory and the
 * first bomb to reach WaterBomb::Behavior faulted jumping to 0x0213b9d8.
 *
 * The dispatcher (WaterBomb::Behavior) calls the state through a void(*)(char*)
 * and ignores r0, so this copy is the identical body declared void: the two
 * value-carrying paths in the original (`return sy;` for py > sy, the implicit
 * fall-through r0) are dead at the only call site. This is the Cannon_Behavior /
 * Coin_Behavior "HOST COPY" idiom -- a matched function transcribed because MSVC
 * will not take the mwccarm form verbatim, with nothing about the effect
 * changed. Delete this and put src/func_ov098_0213b9d8.cpp back in the slice the
 * moment its return type is reconciled.
 */
#include "types.h"
#include "common.h"

struct Vector3_16;
struct dActor_c;
/* Actor::ClosestPlayer() is a __thiscall method in the ROM: `this` rides in a
 * register, and src/func_ov098_0213b9d8.cpp declares it (void) because the
 * decompiler could drop the implicit self. The host definition
 * (src/_ZN8dActor_c13ClosestPlayerEv.cpp) is a cdecl free function that reads its
 * self argument off the STACK (`(char*)c + 0x5c`), so the self MUST be passed
 * here or ClosestPlayer scans from a garbage `this` and Vec3_Dist derefs
 * null+0x5c -- which is the fault this host copy exists to fix. Every other
 * ov098 caller (_ZN16daObjFallBlock_c8BehaviorEv) passes self; only 0213b9d8's own
 * declaration dropped it. */
extern "C" dActor_c *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern "C" s32 Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern "C" dActor_c *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    u32, u32, const Vector3 *, const Vector3_16 *, s32, s32);
extern "C" void func_0201267c(u32 a, void *b, void *c, int d);
extern s16 data_02082214[];
#define M(x) ((long long)(int)(x))

/* PORT_HOST_ABI: MSVC C2561 rejects the matched int-with-bare-return form;
   identical body declared void, r0 dead at the only call site. */
extern "C" void func_ov098_0213b9d8(char *self)
{
    Vector3 v; dActor_c *player; s32 ang; s32 vel; u32 ua; int factor; int a; s16 sn, cs;
    int nx, ny, nz; int py, sy;
    if (*(u16 *)(self + 0x100)) return;
    player = _ZN8dActor_c13ClosestPlayerEv(self);
    if (!player) return;
    { char *p = (char *)(int)M((char *)player + 0x5c);
      v.x=*(int*)p; v.y=*(int*)(p+4); v.z=*(int*)(p+8); }
    if (Vec3_HorzDist(&v, (Vector3 *)(self + 0x5c)) > 0x5dc000) return;
    py = v.y;
    sy = *(int *)(self + 0x60);
    if (py > sy) return;
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
    { dActor_c *spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(0xd0,1,&v,0,*(signed char*)(self+0xcc),-1);
      *(int*)((char*)spawned+0x98)=0; }
    *(u16*)(self+0x100)=0x96;
    func_0201267c(0xd8, self+0x74, self+0x100, 0x96);
}
