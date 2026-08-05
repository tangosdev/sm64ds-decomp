//cpp
#include "types.h"
// @symbol _ZN5Crate8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Crate.h"
/* _ZN5Crate8BehaviorEv at 0x02139b0c */
enum Bool { FALSE, TRUE };

extern u32 data_0209b454;
extern "C" {
extern void _ZN6Player9DropActorEv(void* self);
extern int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern int _ZN5Actor13DistToCPlayerEv(void* self);
extern void Crate_SetState(char* c, int i);
extern u8 DecIfAbove0_Byte(u8* p);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int c, int d, int e, const void* v, void* cb);
extern void* _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(u32 a, u32 b, int c, int d, int e, const void* v);
}

int Crate::Behavior()
{
    struct Vector3 v;
    struct Vector3 vec;
    struct Vector3 t;
    struct Vector3 vec2;
    enum Bool b1, b2;
    int x, y, z;

    b1 = (enum Bool)((unk_0b0 & 0x4000000) != 0);
    if (b1 != FALSE && (data_0209b454 & 0x4000000) && *(void**)((char*)&unk_5e4)) {
        _ZN6Player9DropActorEv(*(void**)((char*)&unk_5e4));
    }

    b2 = (enum Bool)((unk_0b0 & 8) != 0);
    if (b2 != FALSE
          && Vec3_HorzDist((struct Vector3*)((char*)&mPosX), (const struct Vector3*)((char*)&unk_4e8))
          && _ZN5Actor13DistToCPlayerEv(((char*)this)) > 0x7d0000) {
        Crate_SetState(((char*)this), 6);
        return 1;
    }

    if (unk_606 != 0) {
        x = mPosX;
        z = mPosZ;
        y = mPosY + 0x50000;
        ((int*)&v)[0] = x;
        ((int*)&v)[1] = y;
        ((int*)&v)[2] = z;
        if (DecIfAbove0_Byte((u8*)((char*)&unk_606))) {
            *(void**)((char*)&unk_5fc) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unk_5fc, 0x13a, v.x, v.y, v.z, 0, 0);
            *(void**)((char*)&unk_600) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unk_600, 0x13b, v.x, v.y, v.z, 0);
            goto done;
        }
        func_ov098_02138e08(((char*)this));
        x = mPosX;
        z = mPosZ;
        y = mPosY + 0x28000;
        vec.x = x;
        vec.y = y;
        vec.z = z;
        ((int*)&vec2)[0] = ((int*)&vec)[0];
        ((int*)&vec2)[1] = ((int*)&vec)[1];
        ((int*)&vec2)[2] = ((int*)&vec)[2];
        _ZN5Actor19DisappearPoofDustAtERK7Vector3(((char*)this), &vec2);
        Crate_SetState(((char*)this), 6);
        return 1;
    }

done:
    ((int*)&t)[0] = mPosX;
    ((int*)&t)[1] = mPosY;
    ((int*)&t)[2] = mPosZ;
    ((int*)&t)[1] = t.y - unk_5f4;
    unk_598 = t.x;
    unk_59c = t.y;
    unk_5a0 = t.z;
    unk_5d8 = t.x;
    unk_5dc = t.y;
    unk_5e0 = t.z;
    func_ov098_02138b70(((char*)this));
    return 1;
}
