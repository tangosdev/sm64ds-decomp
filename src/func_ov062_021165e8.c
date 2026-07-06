extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern int func_ov062_02115f84(void* c);
extern void func_ov062_02116cd8(void* c, void* p);
extern int _ZN5Actor18HorzAngleToCPlayerEv(void* self);
extern int ApproachAngle(short* angle, int target, int a, int b, int c);
extern int AngleDiff(int a, int b);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, const void* v, unsigned int e);
extern int Vec3_Dist(const void* a, const void* b);

extern char data_ov062_0211df00;
extern char data_ov062_0211de70;
extern char data_ov062_0211ded0;

struct Vec3 { int x, y, z; };

int func_ov062_021165e8(char* c)
{
    char* player;
    int r;
    volatile struct Vec3 pos;
    struct Vec3* pp;

    player = (char*)_ZN5Actor13ClosestPlayerEv(c);
    r = func_ov062_02115f84(c);
    if (r != 0 || *(unsigned char*)(c + 0x3e4) == 1) {
        if (r != 2)
            func_ov062_02116cd8(c, &data_ov062_0211df00);
        else
            func_ov062_02116cd8(c, &data_ov062_0211de70);
        return 1;
    }

    if (player == 0)
        return 1;

    pp = (struct Vec3*)(int)(((long long)(int)(player + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
    pos.x = pp->x;
    pos.y = pp->y;
    pos.z = pp->z;

    if (*(int*)(c + 0x3f0) == 0) {
        *(short*)(c + 0x3f4) = (short)_ZN5Actor18HorzAngleToCPlayerEv(c);
        ApproachAngle((short*)(c + 0x94), *(short*)(c + 0x3f4), 0x80, 0x200, 0x400);
        if (AngleDiff(*(short*)(c + 0x3f4), *(short*)(c + 0x8e)) < 0x200) {
            *(int*)(c + 0x98) = 0x1e000;
            *(int*)(c + 0x3f0) = 1;
            *(int*)(c + 0x3cc) = pos.x;
            *(int*)(c + 0x3d0) = pos.y;
            *(int*)(c + 0x3d4) = pos.z;
        }
    }

    *(unsigned int*)(c + 0x3ec) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int*)(c + 0x3ec), 3, 0x18a, (const void*)(c + 0x74), 0);

    {
        int s = *(int*)(c + 0x3f0);
        if (s != 0) {
            if (s == 1) {
                if (Vec3_Dist((const void*)(c + 0x5c), (const void*)(c + 0x3cc)) < 0x3c000)
                    *(int*)(c + 0x3f0) = 2;
            } else {
                if (Vec3_Dist((const void*)(c + 0x5c), (const void*)(c + 0x3cc)) > 0xc8000)
                    func_ov062_02116cd8(c, &data_ov062_0211ded0);
            }
        }
    }
    return 1;
}
