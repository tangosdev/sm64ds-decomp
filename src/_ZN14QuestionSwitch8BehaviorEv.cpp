//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

extern "C" {
unsigned short DecIfAbove0_Short(unsigned short* p);
unsigned char DecIfAbove0_Byte(unsigned char* p);
void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e);
int _ZN6Player12GetTalkStateEv(void* self);
int _ZN9Animation8FinishedEv(void* self);
void _ZN9Animation7AdvanceEv(void* self);
void _ZN7Message11PrepareTalkEv(void);
void _ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(void* self, void* actor, unsigned int msg, const void* vec, unsigned int a, unsigned int b);
void _ZN7Message13DisplaySavingEt(unsigned short a);
int _ZN6Player18HasFinishedTalkingEv(void* self);
void _ZN7Message7EndTalkEv(void);
char* _ZN5Actor13ClosestPlayerEv(void* self);
int _ZN6Player9StartTalkER9ActorBaseb(void* self, void* actor, int b);
int _ZN16MeshColliderBase9IsEnabledEv(void* self);
void _ZN16MeshColliderBase7DisableEv(void* self);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
int func_ov002_020b4fd0(char* c);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const void* v);
void func_ov002_020b50a0(char* c);
void func_ov002_020b503c(char* self);
}

extern u8 data_0209d684;
extern u8 data_0209d660;
extern u32 data_0209caa0[];

extern "C" int _ZN14QuestionSwitch8BehaviorEv(char* self)
{
    if (*(u16*)(self + 0x71c) != 0) {
        if (DecIfAbove0_Short((u16*)(self + 0x71c)) == 0) {
            _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x7f, 0, 0x8777, 0);
        }
    }

    if (*(u8*)(self + 0x71b) != 0) {
        char* player = *(char**)(self + 0x720);
        if (player != 0) {
            switch (_ZN6Player12GetTalkStateEv(player)) {
            case 0:
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x6b000, 0);
                *(u16*)(self + 0x71c) = 0x4b;
                if (_ZN9Animation8FinishedEv(self + 0x704) != 0) {
                    _ZN7Message11PrepareTalkEv();
                    _ZN6Player11ShowMessageER9ActorBasejPK7Vector3hh(player, self, 0x183, 0, 1, 2);
                }
                break;
            case 1:
                break;
            default: {
                u8 st = data_0209d684;
                if (st == 1) {
                    _ZN7Message13DisplaySavingEt(0x295);
                } else if (st == 2) {
                    if (_ZN6Player18HasFinishedTalkingEv(player) != 0) {
                        _ZN7Message7EndTalkEv();
                        *(int*)(self + 0x720) = 0;
                        *(u8*)(self + 0x71b) = 0;
                    }
                } else {
                    if (data_0209d660 == 0) {
                        if (_ZN6Player18HasFinishedTalkingEv(player) != 0) {
                            _ZN7Message7EndTalkEv();
                            *(int*)(self + 0x720) = 0;
                            *(u8*)(self + 0x71b) = 0;
                        }
                    }
                }
                break;
            }
            }
        } else {
            char* p = _ZN5Actor13ClosestPlayerEv(self);
            if (*(u8*)(p + 0x6de) == 0) {
                if (_ZN6Player9StartTalkER9ActorBaseb(p, self, 1) != 0) {
                    *(char**)(self + 0x720) = p;
                }
            }
        }
    }

    {
        u8 f = *(u8*)(self + 0x71a);
        if (f != 0) {
            if (DecIfAbove0_Byte((u8*)(self + 0x718)) == 0) {
                if (_ZN16MeshColliderBase9IsEnabledEv(*(char**)(self + 0x320)) != 0) {
                    _ZN16MeshColliderBase7DisableEv(*(char**)(self + 0x320));
                }
                *(char**)(self + 0x320) = self + 0x4ec;
                data_0209caa0[1] |= 0x80000000;
                {
                    volatile Vector3 v;
                    int y = *(int*)(self + 0x60) + 0x64000;
                    int z = *(int*)(self + 0x64);
                    int x = *(int*)(self + 0x5c);
                    v.x = x;
                    v.y = y;
                    v.z = z;
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x6d, x, y, z);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x6e, v.x, v.y, v.z);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x6f, v.x, v.y, v.z);
                }
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x70, *(int*)(self + 0x5c), *(int*)(self + 0x60), *(int*)(self + 0x64));
                *(u8*)(self + 0x71b) = 1;
            }
        } else if (f == 0) {
            *(u8*)(self + 0x718) = 8;
        }
    }

    if ((data_0209caa0[1] & 0x80000000) == 0) {
        *(char**)(self + 0x320) = self + 0x324;
    }

    if (func_ov002_020b4fd0(self) == 0) {
        if (*(char**)(self + 0x320) == self + 0x324) {
            *(int*)(self + 0x70c) = 0;
        } else {
            if (_ZN9Animation8FinishedEv(self + 0x704) == 0) {
                _ZN9Animation7AdvanceEv(self + 0x704);
                if (_ZN9Animation8FinishedEv(self + 0x704) != 0) {
                    _ZN5Sound9PlayBank3EjRK7Vector3(0, self + 0x74);
                }
            }
        }
        func_ov002_020b50a0(self);
        func_ov002_020b503c(self);
    }

    *(u8*)(self + 0x71a) = 0;
    return 1;
}
