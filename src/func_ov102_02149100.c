typedef struct { int x, y, z; } Vector3;
typedef struct { short x, y, z; } Vector3_16;

extern int RandomIntInternal(int* seed);
extern char* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, const Vector3* v, const Vector3_16* w, int e, int f);
extern int data_0209e650;

void func_ov102_02149100(char *c, Vector3 *pos, int n, unsigned int speed, short arg5)
{
    char *a;
    int dir;
    unsigned int rnd;
    unsigned int q;
    int prevDir = 0xff;
    int i = 0;

    if (n <= 0) return;

    do {
        a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x120, 2, pos, 0, *(signed char*)(c+0xcc), -1);
        if (a != 0) {
            do {
                rnd = (unsigned int)RandomIntInternal(&data_0209e650);
                dir = (int)((rnd >> 0x10) << 0x1b) >> 0x10;
            } while (dir == prevDir);
            rnd = (unsigned int)RandomIntInternal(&data_0209e650);
            *(short*)(a+0x92) = 0;
            q = rnd >> 0x10;
            speed = (speed * (q % 50 + 100)) / 100;
            prevDir = dir;
            *(short*)(a+0x94) = arg5 + dir;
            *(short*)(a+0x96) = 0;
            *(unsigned int*)(a+0x98) = speed;
        }
        i++;
    } while (i < n);
}
