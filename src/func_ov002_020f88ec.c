extern int RandomIntInternal(int* seed);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void* v, void* w, int e, int f);
extern int RNG_STATE;

void func_ov002_020f88ec(char* c)
{
    char* a;
    if (((unsigned int)RandomIntInternal(&RNG_STATE) >> 0x10) % 10 >= 4) return;
    a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x120, 0, c + 0x5c, 0, *(signed char*)(c + 0xcc), -1);
    if (a != 0) {
        *(int*)(a + 0xa4) = 0;
        *(int*)(a + 0xa8) = 0;
        *(int*)(a + 0xac) = 0;
    }
}
