//cpp
struct Vector3 {
    int x, y, z;
    Vector3(int a, int b, int c) : x(a), y(b), z(c) {}
    ~Vector3() {}
};
typedef struct { int a, b; } P2;

extern "C" {
extern P2 data_ov065_0211d6a0;
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, int file, int frame, int speed, unsigned int flags);
extern void func_02012694(int id, void *pos);
extern int func_ov065_021177e4(int *t);
}

int func_ov065_021177e4(int *t)
{
    t[0x27] = -0x1000;
    t[0x28] = -0xa000;
    *((char *)t + 0x43c) = 0;
    t[0xf0] = 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)t + 0x364, data_ov065_0211d6a0.b, 0, 0x1000, 0);
    Vector3 vec(t[0x17], t[0x18] + 0x64000, t[0x19]);
    *(short *)((char *)t + 0x100) = 0x19;
    if (t[0x18] == 0x898000)
        *(short *)((char *)t + 0x100) = 0x32;
    func_02012694(0xf9, (char *)t + 0x74);
    return 1;
}
