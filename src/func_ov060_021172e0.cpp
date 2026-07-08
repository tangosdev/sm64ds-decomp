//cpp
typedef unsigned int u32;
typedef int s32;
typedef int Fix12i;
typedef short s16;
typedef signed char s8;

struct Vector3 { Fix12i x, y, z; };
struct Vector3_16 { s16 x, y, z; };

extern "C" {
    void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
    void _ZN9ActorBase18MarkForDestructionEv(void* self);
    int RandomIntInternal(int* seed);
    void* _ZN5Actor13ClosestPlayerEv(void* self);
    void* _ZN5Actor15FindWithActorIDEjPS_(u32 id, void* prev);
    int _ZN8SaveData19IsCharacterUnlockedEj(u32 c);
    void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        u32 actorID, u32 param1, const struct Vector3* pos,
        const struct Vector3_16* rot, s32 areaID, s32 deathTableID);
}

extern int RNG_STATE;

extern "C" void func_ov060_021172e0(void* self)
{
    char* sl = (char*)self;

    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x9f, *(Fix12i*)(sl + 0x5c), *(Fix12i*)(sl + 0x60), *(Fix12i*)(sl + 0x64));
    _ZN9ActorBase18MarkForDestructionEv(sl);

    if ((((u32)RandomIntInternal(&RNG_STATE) >> 0x10) % 10) >= 2) {
        return;
    }

    {
        void* p = _ZN5Actor13ClosestPlayerEv(sl);
        char* sb;
        if (p != 0 && *(int*)((char*)p + 8) == 3 &&
            (sb = (char*)_ZN5Actor15FindWithActorIDEjPS_(0x117, 0)) != 0 &&
            *(unsigned char*)(sb + 0x42b) == 0) {
            int c;
            int idx;
            int mask = 0;
            for (c = 0; c < 3; c++) {
                if (_ZN8SaveData19IsCharacterUnlockedEj(c) != 0) {
                    mask = (mask | (1 << c)) & 0xff;
                }
            }
            do {
                idx = ((u32)RandomIntInternal(&RNG_STATE) >> 0x10) % 3;
            } while ((mask & (1 << idx)) == 0);
            {
                void* r = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                    0x10d, (idx << 8) | 0xb, (struct Vector3*)(sl + 0x5c),
                    (struct Vector3_16*)(sl + 0x8c), *(s8*)(sl + 0xcc), -1);
                if (r != 0) {
                    *(unsigned char*)(sb + 0x42b) = 1;
                }
            }
            return;
        }
    }

    {
        void* r = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            0x120, 0, (struct Vector3*)(sl + 0x5c), 0, *(s8*)(sl + 0xcc), -1);
        if (r != 0) {
            *(int*)((char*)r + 0xa4) = 0;
            *(int*)((char*)r + 0xa8) = 0;
            *(int*)((char*)r + 0xac) = 0;
        }
    }
}
