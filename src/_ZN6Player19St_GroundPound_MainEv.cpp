//cpp
typedef signed char s8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

extern "C" {
int _ZN6Player12FinishedAnimEv(char* self);
void _ZN6Player7SetAnimEji5Fix12IiEj(char* self, unsigned int anim, int a, int b, unsigned int c);
void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const Vector3* v);
void func_ov002_020ef2a4(char* c, char* arg);
void func_ov002_020c2f64(char* c);
void func_ov002_020c0364(char* c, u32 arg);
int func_ov002_020e2c84(char* self);
void func_ov002_020dd908(char* sb);
void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, const Vector3* v);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void func_0200d8c8(void* cam, const void* v, int strength);
void func_ov002_020dbc94(char* c);
void _ZN12CylinderClsn5ClearEv(char* self);
void _ZN12CylinderClsn6UpdateEv(char* self);
void _ZN6Player11ChangeStateERNS_5StateE(char* self, void* state);
int func_ov002_020d36d8(char* c, int arg);
void Player_AdvanceAnims(char* self);
}

extern void* data_0209f318;
extern u8 data_020a0e40;
extern char data_0209f4a0[];
extern int data_ov002_0211013c;

#pragma opt_strength_reduction off
#pragma opt_common_subs off

extern "C" int _ZN6Player19St_GroundPound_MainEv(char* self)
{
    s8 timer = *(s8*)(self + 0x6e3);
    if (timer > 0) {
        *(int*)(self + 0xa8) = 0;
        if (*(u8*)(self + 0x6e3) == 2)
            *(int*)(self + 0xa8) = 0xa000;
        if (_ZN6Player12FinishedAnimEv(self) != 0) {
            *(u8*)((((int)self) + 0x6e3) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
            if (*(u8*)(self + 0x6e3) != 0) {
                u32 anim = 0x3c;
                if (*(u8*)(self + 0x703) != 0) anim = 0xa2;
                _ZN6Player7SetAnimEji5Fix12IiEj(self, anim, 0x40000000, 0x1000, 0);
            } else {
                u32 anim = 0x3d;
                if (*(u8*)(self + 0x703) != 0) anim = 0xa1;
                _ZN6Player7SetAnimEji5Fix12IiEj(self, anim, 0x40000000, 0x1000, 0);
                *(int*)(self + 0xa8) = -0x32000;
                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(self + 0x6d9), 0x15, (Vector3*)(self + 0x74));
                *(int*)(self + 0x684) = *(int*)(self + 0x60);
            }
        }
    } else if (timer == 0) {
        u16 st;
        func_ov002_020ef2a4(self + 0x380, self);
        st = *(u16*)(self + 0x6a4);
        if (st != 0) {
            if (st == 1) {
                func_ov002_020c2f64(self);
                _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x42, 0x40000000, 0x1000, 0);
                *(u8*)(self + 0x6e3) = 0xff;
                *(u32*)((((int)self) + 0x2ec) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x20;
            }
            {
                u16 f = *(u16*)(self + 0x6ce) & 1;
                if (f != 0) {
                    func_ov002_020c0364(self, 1);
                    return 1;
                }
            }
        } else {
            if (*(u8*)(self + 0x6de) == 0) {
                if (func_ov002_020e2c84(self) == 2)
                    return 1;
                func_ov002_020dd908(self);
                if (*(u8*)(self + 0x703) != 0) {
                    _ZN5Sound9PlayBank0EjRK7Vector3(0xd3, (Vector3*)(self + 0x74));
                } else {
                    _ZN5Sound9PlayBank0EjRK7Vector3(*(u32*)(self + 0x66c) + 0x90, (Vector3*)(self + 0x74));
                }
                {
                    Vector3 v;
                    int x = *(int*)(self + 0x5c);
                    v.x = x;
                    v.y = *(int*)(self + 0x60);
                    int z = *(int*)(self + 0x64);
                    v.z = z;
                    if (*(u8*)(self + 0x703) != 0) {
                        int yy = *(int*)(self + 0x60) + 0x14000;
                        v.y = yy;
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x57, x, yy, z);
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x58, v.x, v.y, v.z);
                        if (*(u8*)(self + 0x707) == 0) {
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5a, v.x, v.y, v.z);
                            int y2 = *(int*)(self + 0x60) + 0x3c000;
                            int xx = v.x;
                            int zz = v.z;
                            *(volatile int*)&v.y = y2;
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x59, xx, y2, zz);
                        }
                    } else if (*(int*)(self + 8) != 2) {
                        int yy = *(int*)(self + 0x60) + 0xa000;
                        v.y = yy;
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd3, x, yy, z);
                        if (*(u8*)(self + 0x707) == 0) {
                            int y2 = *(int*)(self + 0x60) + 0x3c000;
                            int xx = v.x;
                            int zz = v.z;
                            v.y = y2;
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd4, xx, y2, zz);
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd5, v.x, v.y, v.z);
                        }
                    } else {
                        int yy = *(int*)(self + 0x60) + 0xa000;
                        v.y = yy;
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x134, x, yy, z);
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x135, v.x, v.y, v.z);
                        if (*(u8*)(self + 0x707) == 0) {
                            int y2 = *(int*)(self + 0x60) + 0x32000;
                            int xx = v.x;
                            int zz = v.z;
                            v.y = y2;
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x136, xx, y2, zz);
                            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x137, v.x, v.y, v.z);
                        }
                    }
                }
                func_0200d8c8(data_0209f318, self + 0x5c, 0x578000);
                *(u16*)(self + 0x6a4) = 6;
            } else {
                func_ov002_020dbc94(self);
                _ZN12CylinderClsn5ClearEv(self + 0x314);
                _ZN12CylinderClsn6UpdateEv(self + 0x314);
            }
        }
    } else {
        if (*(short*)(data_0209f4a0 + data_020a0e40 * 0x18) != 0 || _ZN6Player12FinishedAnimEv(self) != 0) {
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211013c);
            return 1;
        }
        func_ov002_020d36d8(self, 1);
    }
    Player_AdvanceAnims(self);
    return 1;
}
