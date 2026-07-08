//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Vector3 { s32 x, y, z; };
struct Vector3_16;

extern "C" {
    void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 a, const Vector3* pos, const Vector3_16* rot, int p0, int p1);
    void* _ZN5Actor10FindWithIDEj(u32 id);
}

extern u8 CURRENT_GAMEMODE[];

extern "C" void func_ov100_021435e8(char* c)
{
    if (*(u8*)(c + 0x6c8) != 0) return;

    int flag = (CURRENT_GAMEMODE[0] == 1);
    if (!flag) {
        if (*(s32*)(c + 0x6d0) == 0) {
            void* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb3, 0,
                (Vector3*)(c + 0x714), 0, *(signed char*)(c + 0xcc), -1);
            if (a != 0) *(s32*)(c + 0x6d0) = *(s32*)((char*)a + 4);
        } else {
            void* a = _ZN5Actor10FindWithIDEj(*(s32*)(c + 0x6d0));
            if (a != 0) {
                if (*(s32*)((char*)a + 0x440) == 5) {
                    *(u8*)(c + 0x6c8) = 1;
                    *(s32*)(c + 0x6d0) = 0;
                    return;
                }
                *(s32*)((char*)a + 0x5c) = *(s32*)(c + 0x714);
                *(s32*)((char*)a + 0x60) = *(s32*)(c + 0x718);
                *(s32*)((char*)a + 0x64) = *(s32*)(c + 0x71c);
                return;
            }
            *(u8*)(c + 0x6c8) = 1;
            *(s32*)(c + 0x6d0) = 0;
        }
    } else {
        if (*(s32*)(c + 0x6d0) == 0) {
            void* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb2, *(u32*)(c + 0x6b8) | 0x30,
                (Vector3*)(c + 0x714), 0, *(signed char*)(c + 0xcc), -1);
            if (a != 0) *(s32*)(c + 0x6d0) = *(s32*)((char*)a + 4);
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb4, *(u32*)(c + 0x6b8) | 0x30,
                (Vector3*)(c + 0x714), 0, *(signed char*)(c + 0xcc), -1);
        } else {
            void* a = _ZN5Actor10FindWithIDEj(*(s32*)(c + 0x6d0));
            if (a != 0) {
                if (*(s32*)((char*)a + 0x440) == 5) {
                    *(u8*)(c + 0x6c8) = 1;
                    *(s32*)(c + 0x6d0) = 0;
                    return;
                }
                *(s32*)((char*)a + 0x5c) = *(s32*)(c + 0x714);
                *(s32*)((char*)a + 0x60) = *(s32*)(c + 0x718);
                *(s32*)((char*)a + 0x64) = *(s32*)(c + 0x71c);
                return;
            }
            *(u8*)(c + 0x6c8) = 1;
            *(s32*)(c + 0x6d0) = 0;
        }
    }
}
