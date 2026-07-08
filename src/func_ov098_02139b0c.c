/* func_ov098_02139b0c at 0x02139b0c */
typedef unsigned int u32;
typedef unsigned char u8;

struct Vector3 { int x, y, z; };

enum Bool { FALSE, TRUE };

extern u32 NEXT_ACTOR_UPDATE_FLAGS;
extern void _ZN6Player9DropActorEv(void* self);
extern int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern int _ZN5Actor13DistToCPlayerEv(void* self);
extern void func_ov098_02138b28(char* c, int i);
extern u8 DecIfAbove0_Byte(u8* p);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int c, int d, int e, const void* v, void* cb);
extern void* _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(u32 a, u32 b, int c, int d, int e, const void* v);
extern void func_ov098_02138e08(char* c);
extern void _ZN5Actor19DisappearPoofDustAtERK7Vector3(void* self, const struct Vector3* vec);
extern void func_ov098_02138b70(char* c);

int func_ov098_02139b0c(char* self)
{
    struct Vector3 v;
    struct Vector3 vec;
    struct Vector3 t;
    struct Vector3 vec2;
    enum Bool b1, b2;
    int x, y, z;

    b1 = (enum Bool)((*(u32*)(self + 0xb0) & 0x4000000) != 0);
    if (b1 != FALSE && (NEXT_ACTOR_UPDATE_FLAGS & 0x4000000) && *(void**)(self + 0x5e4)) {
        _ZN6Player9DropActorEv(*(void**)(self + 0x5e4));
    }

    b2 = (enum Bool)((*(u32*)(self + 0xb0) & 8) != 0);
    if (b2 != FALSE
          && Vec3_HorzDist((struct Vector3*)(self + 0x5c), (const struct Vector3*)(self + 0x4e8))
          && _ZN5Actor13DistToCPlayerEv(self) > 0x7d0000) {
        func_ov098_02138b28(self, 6);
        return 1;
    }

    if (*(u8*)(self + 0x606) != 0) {
        x = *(int*)(self + 0x5c);
        z = *(int*)(self + 0x64);
        y = *(int*)(self + 0x60) + 0x50000;
        ((int*)&v)[0] = x;
        ((int*)&v)[1] = y;
        ((int*)&v)[2] = z;
        if (DecIfAbove0_Byte((u8*)(self + 0x606))) {
            *(void**)(self + 0x5fc) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(u32*)(self + 0x5fc), 0x13a, v.x, v.y, v.z, 0, 0);
            *(void**)(self + 0x600) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(*(u32*)(self + 0x600), 0x13b, v.x, v.y, v.z, 0);
            goto done;
        }
        func_ov098_02138e08(self);
        x = *(int*)(self + 0x5c);
        z = *(int*)(self + 0x64);
        y = *(int*)(self + 0x60) + 0x28000;
        vec.x = x;
        vec.y = y;
        vec.z = z;
        ((int*)&vec2)[0] = ((int*)&vec)[0];
        ((int*)&vec2)[1] = ((int*)&vec)[1];
        ((int*)&vec2)[2] = ((int*)&vec)[2];
        _ZN5Actor19DisappearPoofDustAtERK7Vector3(self, &vec2);
        func_ov098_02138b28(self, 6);
        return 1;
    }

done:
    ((int*)&t)[0] = *(int*)(self + 0x5c);
    ((int*)&t)[1] = *(int*)(self + 0x60);
    ((int*)&t)[2] = *(int*)(self + 0x64);
    ((int*)&t)[1] = t.y - *(int*)(self + 0x5f4);
    *(int*)(self + 0x598) = t.x;
    *(int*)(self + 0x59c) = t.y;
    *(int*)(self + 0x5a0) = t.z;
    *(int*)(self + 0x5d8) = t.x;
    *(int*)(self + 0x5dc) = t.y;
    *(int*)(self + 0x5e0) = t.z;
    func_ov098_02138b70(self);
    return 1;
}
