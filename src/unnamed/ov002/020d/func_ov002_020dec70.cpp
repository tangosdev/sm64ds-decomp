//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

struct Vector3_16f { s16 x, y, z; };
struct Vector3 { int x, y, z; };

extern "C" {
void func_ov002_020bf800(char *c, Vector3_16f v);
u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, Vector3 *v, u32 d);
int _ZN6Player7IsStateERNS_5StateE(char *c, void *st);
int func_ov002_020e0478(void *c);
void _ZN6Player11ChangeStateERNS_5StateE(char *c, void *st);
}
extern char data_ov002_02110274;
extern char data_ov002_021102d4;
extern char data_ov002_02110244;

extern "C" int func_ov002_020dec70(char *c)
{
    int t = *(int *)(c + 0x60) + 0x514000;
    if (t < (int)0xfdecc000) return 0;
    {
        Vector3_16f v;
        v.z = 0;
        v.y = 0x1000;
        v.x = v.z;
        func_ov002_020bf800(c, v);
    }
    *(u32 *)(c + 0x624) = _ZN5Sound8PlayLongEjjjRK7Vector3s(*(u32 *)(c + 0x624), 3, 0x90, (Vector3 *)(c + 0x74), 0);
    if (_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110274)
        || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102d4)
        || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110244)
        || *(u8 *)(c + 0x6f9)
        || *(u8 *)(c + 0x703)
        || *(u8 *)(c + 0x709)
        || *(u8 *)(c + 0x6de) == 0) return 0;
    if (func_ov002_020e0478(c) != 0 || *(u8 *)(c + 0x6fd) != 0) {
        if (t < 0x898000) *(int *)(c + 0x60) += 0x8000;
        return 0;
    }
    _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110274);
    return 1;
}
