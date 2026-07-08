//cpp
typedef int Fix12i;
struct Vector3 { int x, y, z; };
struct Vector3_16;

extern "C" void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    unsigned int a, unsigned int b, const Vector3&, const Vector3_16*, int, int);

extern unsigned char CURRENT_GAMEMODE[];
extern Vector3 data_ov009_02113de0[];
extern Vector3 data_ov009_02113e34[];
extern Vector3 data_ov009_02113d8c[];
struct D0209caa0 { int a, b, c; };
extern D0209caa0 SAVE_DATA;

extern "C" void func_ov009_02111b1c(char* thiz)
{
    char* c = thiz;
    Vector3* vec;
    int i;
    int b = (int)(CURRENT_GAMEMODE[0] == 1);
    if (b != 0) {
        if (*(int*)(c + 0x5c) > 0) return;
        vec = data_ov009_02113de0;
    } else {
        vec = (SAVE_DATA.c & 0x80000) ? data_ov009_02113e34 : data_ov009_02113d8c;
    }
    for (i = 0; i < 7; i++) {
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
            0xc5, 0, vec[i], 0, *(signed char*)(c + 0xcc), -1);
    }
}
