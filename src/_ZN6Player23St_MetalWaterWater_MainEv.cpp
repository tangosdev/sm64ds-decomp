//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

struct Vector3 { int x, y, z; };
struct S18 { short a; short b; int c[5]; };

extern "C" {
extern int func_ov002_020cec2c(char *c);
extern void _ZN6Player11ChangeStateERNS_5StateE(char *c, void *s);
extern int func_ov002_020eeca8(void *a, void *b);
extern int func_ov002_020bf224(void *c, int a, int b);
extern void ApproachAngle(short *p, int target, int a, int b, int d);
extern void _Z14ApproachLinearRiii(int *v, int a, int b);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void *v);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int x, int y, int z);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(char *c, u32 a, int b, int d, u32 e);
extern int _ZN6Player12FinishedAnimEv(char *c);
extern void func_ov002_020bedd4(char *c);

extern u8 data_020a0e40;
extern struct S18 data_0209f4a0[];
extern int data_0209f32c;
extern int data_ov002_0211067c;
extern int data_ov002_021106ac;
}

extern "C" int _ZN6Player23St_MetalWaterWater_MainEv(char *c)
{
    volatile Vector3 pos;
    u32 f;
    int v;
    int lim;
    int py;
    int px, py2, pz;

    if (func_ov002_020cec2c(c) != 0) return 1;
    f = *(u8 *)(c + 0x6f9);
    if (f == 0 && *(u8 *)(c + 0x703) == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211067c);
        return 1;
    }
    if (f != 0) func_ov002_020eeca8(c + 0x380, c);

    if (*(u8 *)(c + 0x6e5) == 0) {
        v = 0;
        if (data_0209f4a0[data_020a0e40].a != 0) {
            v = func_ov002_020bf224(c, 0x14000, 0x1000);
            ApproachAngle((short *)(c + 0x94), *(short *)(c + 0x6d2), 0x20, 0x1000, 0x10);
        }
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        _Z14ApproachLinearRiii((int *)(c + 0x98), v, 0x400);
        py = *(int *)(c + 0x60);
    lim = py + 0x46000;
    if (*(u8 *)(c + 0x703) != 0) lim = py + 0xd2000;
        if (data_0209f32c > lim) {
            *(int *)(c + 0x9c) = -0x1800;
            *(int *)(c + 0xa0) = -0x14000;
        } else {
            *(int *)(c + 0x9c) = -0x4000;
            *(int *)(c + 0xa0) = -0x4b000;
        }
        if (*(u8 *)(c + 0x6de) == 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(0xa9, c + 0x74);
            px = *(int *)(c + 0x5c);
            pos.x = px;
            py2 = *(int *)(c + 0x60);
            pos.y = py2;
            pz = *(int *)(c + 0x64);
            pos.z = pz;
            if (*(u8 *)(c + 0x703) != 0) {
                py2 += 0x19000;
                pos.y = py2;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5c, px, py2, pz);
            } else {
                py2 += 0x32000;
                pos.y = py2;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd7, px, py2, pz);
            }
            if (data_0209f4a0[data_020a0e40].a != 0) {
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021106ac);
            } else {
                *(int *)(c + 0x98) = 0;
                if (*(u8 *)(c + 0x6e3) != 0)
                    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x52, 0x40000000, 0x1000, 0);
                else
                    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x55, 0x40000000, 0x1000, 0);
                *(u8 *)(c + 0x6e5) = 1;
            }
        }
    } else {
        if (_ZN6Player12FinishedAnimEv(c) != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021106ac);
            return 1;
        }
    }
    func_ov002_020bedd4(c);
    return 1;
}
