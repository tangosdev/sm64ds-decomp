typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef signed char s8;

struct Vec3 { int x, y, z; };
struct Vec3_16 { s16 x, y, z; };

extern int _ZN5Actor13DistToCPlayerEv(void* self);
extern unsigned int RandomIntInternal(void* g);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, const struct Vec3* pos, const struct Vec3_16* rot, int e, int f);

extern int RNG_STATE;

void func_ov100_021412d8(char* sl)
{
    int typ = *(int*)(sl + 8) & 0xff;
    if ((typ & 0xc0) != 0) {
        *(u8*)(sl + 0x3f0) = (u8)(typ >> 6);
    } else {
        if (_ZN5Actor13DistToCPlayerEv(sl) >= 0xc8000)
            return;
        {
            int sb = (int)(RandomIntInternal(&RNG_STATE) % 3);
            int sel;
            int i;
            int mask = typ & 0x30;
            struct Vec3_16 rot;
            int two = 2;
            int three = 3;
            i = 1;
            rot = *(struct Vec3_16*)(sl + 0x92);
            for (; i < 3; i++) {
                if (i == sb) {
                    sel = 1;
                } else if (mask == 0x20) {
                    sel = two;
                } else {
                    sel = three;
                }
                rot.y = (s16)(rot.y + (s16)(RandomIntInternal(&RNG_STATE) >> 0x10));
                _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x150, mask | (sel << 6),
                    (struct Vec3*)(sl + 0x5c), &rot, (int)*(s8*)(sl + 0xcc), -1);
            }
            if (sb == 0) {
                *(u8*)(sl + 0x3f0) = 1;
            } else if (mask == 0x20) {
                *(u8*)(sl + 0x3f0) = 2;
            } else {
                *(u8*)(sl + 0x3f0) = 3;
            }
        }
    }

    *(s16*)(sl + 0x300 + 0xec) = (s16)(RandomIntInternal(&RNG_STATE) >> 0x10);
    *(s16*)(sl + 0x94) = (s16)(*(s16*)(sl + 0x300 + 0xec) + (int)((RandomIntInternal(&RNG_STATE) >> 0x10) & 0x3fff));
    *(int*)(sl + 0x98) = (int)((RandomIntInternal(&RNG_STATE) >> 0x10) & 0xfff) * 0xf + 0xf000;
    *(int*)(sl + 0x3e8) = 0;
    *(int*)(sl + 0x3e4) = 5;
}
