//cpp
struct Vector3 { int x, y, z; };

extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern int func_02012694(unsigned int id, const Vector3 *v);
extern char *data_0209f318;
}

extern "C" void func_ov002_020b9450(char *c);
void func_ov002_020b9450(char *c)
{
    Vector3 pos;
    int x = *(int *)(c + 0x5c);
    int y = *(int *)(c + 0x60) + 0x82000;
    int z = *(int *)(c + 0x64);
    ((int *)&pos)[0] = x;
    ((int *)&pos)[1] = y;
    ((int *)&pos)[2] = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
        0x102, ((int *)&pos)[0], ((int *)&pos)[1], ((int *)&pos)[2]);
    func_02012694(0x7d, (const Vector3 *)(c + 0x74));
    *(int *)(c + 0x3c4) = 0;
    *(short *)(c + 0x8e) = *(short *)(data_0209f318 + 0x17c);
    *(char *)(c + 0x3cb) = 0x1b;
}
