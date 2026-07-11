typedef unsigned char u8;
typedef unsigned int u32;
typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct Vector3_16 { short x, y, z; } Vector3_16;

extern int _ZN8SaveData19IsCharacterUnlockedEj(u32);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32, u32, const Vector3*, const Vector3_16*, int, int);
extern int data_0209b284[];
extern u8 data_0209f2d8;
extern u8 data_0209b270;

void* func_0200e5fc(u32 idx) {
    int flag;
    int b;
    void* res;
    volatile u8* pb;
    if (data_0209b284[idx] != 0) return 0;
    flag = 0x11;
    b = (data_0209f2d8 == 2) ? 1 : 0;
    if ((int)(((long long)b) & 0xFFFFFFFFFFFFFFFFLL) != 0) {
        if (_ZN8SaveData19IsCharacterUnlockedEj(idx) == 0) return 0;
        flag = 0;
    }
    res = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xbf, idx | (idx << 3) | (data_0209b270 << 6) | (flag << 8), (const Vector3*)0, (const Vector3_16*)0, -1, -1);
    if (res == 0) return res;
    pb = &data_0209b270;
    ((volatile int*)data_0209b284)[idx] = *((int*)res + 1);
    *pb += 1;
    return res;
}
