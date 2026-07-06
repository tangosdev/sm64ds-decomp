typedef unsigned int u32;
typedef int s32;
typedef short s16;
typedef signed char s8;
typedef unsigned char u8;

struct Vector3 { int x, y, z; };
struct Vector3_16 { s16 x, y, z; };

extern void* _ZN5Actor18ClosestWithActorIDEj(void* self, u32 id);
extern int RandomIntInternal(int* seed);
extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern void* _ZN5Actor15FindWithActorIDEjPS_(u32 id, void* prev);
extern int _ZN8SaveData19IsCharacterUnlockedEj(u32 c);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    u32 actorID, u32 param1, const struct Vector3* pos,
    const struct Vector3_16* rot, s32 areaID, s32 deathTableID);

extern int data_0209e650;

void func_ov002_020f897c(void* self)
{
    char* sl = (char*)self;
    u8 n = (u8)(((u32)RandomIntInternal(&data_0209e650) >> 0x10) % 10);
    if (n >= 4) {
        if (_ZN5Actor18ClosestWithActorIDEj(sl, 0xfe) != 0) return;
    }
    {
        void* p = _ZN5Actor13ClosestPlayerEv(sl);
        char* sb;
        if (p != 0 && *(int*)((char*)p + 8) == 3 &&
            (sb = (char*)_ZN5Actor15FindWithActorIDEjPS_(0x117, 0)) != 0 &&
            *(u8*)(sb + 0x42b) == 0) {
            int c;
            int idx;
            int mask = 0;
            for (c = 0; c < 3; c++) {
                if (_ZN8SaveData19IsCharacterUnlockedEj(c) != 0) {
                    mask = (mask | (1 << c)) & 0xff;
                }
            }
            do {
                idx = ((u32)RandomIntInternal(&data_0209e650) >> 0x10) % 3;
            } while ((mask & (1 << idx)) == 0);
            if (_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x10d, (idx << 8) | 0xb,
                    (struct Vector3*)(sl + 0x5c), (struct Vector3_16*)(sl + 0x8c),
                    *(s8*)(sl + 0xcc), -1) != 0) {
                *(u8*)(sb + 0x42b) = 1;
                return;
            }
        }
    }
    if (n >= 4) return;
    {
        void* r = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x120, 0,
            (struct Vector3*)(sl + 0x5c), 0, *(s8*)(sl + 0xcc), -1);
        if (r != 0) {
            *(int*)((char*)r + 0xa4) = 0;
            *(int*)((char*)r + 0xa8) = 0;
            *(int*)((char*)r + 0xac) = 0;
        }
    }
}
