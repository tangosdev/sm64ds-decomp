//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int d, const Vector3& v, unsigned int e);
extern void _Z14ApproachLinearRiii(int& x, int target, int step);

void func_ov095_02136298(char* c)
{
    *(unsigned int*)(c + 0x340) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(unsigned int*)(c + 0x340), 3, 0x82, *(Vector3*)(c + 0x74), 0);
    _Z14ApproachLinearRiii(*(int*)(c + 0xa8), 0xa000, 0x2000);
    (*(int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF)) += *(int*)(c + 0xa8);
    if (*(int*)(c + 0x60) < *(int*)(c + 0x334)) return;
    *(int*)(c + 0x60) = *(int*)(c + 0x334);
    *(char*)(c + 0x346) = 0;
    if (*(unsigned char*)(c + 0x349) != 0) {
        *(int*)(c + 0x32c) = 3;
        return;
    }
    char* o = *(char**)(c + 0x324);
    if (o != 0) {
        if (*(int*)(o + 0x60) <= *(int*)(c + 0x33c)) {
            *(int*)(c + 0x32c) = 2;
            return;
        }
    }
    *(int*)(c + 0x32c) = 3;
}
}
