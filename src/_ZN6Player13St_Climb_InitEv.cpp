//cpp
typedef int Fix12i;

struct Vector3 {
    int x, y, z;
};

extern int Player_ReleaseHeldActor(char*);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int, unsigned int, const Vector3&);
extern int func_ov002_020e3078(void*, void*);
extern char data_ov002_021106f4[];

class Player {
public:
    int St_Climb_Init();
};

int Player::St_Climb_Init() {
    char* self = (char*)this;
    Player_ReleaseHeldActor(self);

    int* bitfield = (int*)(((int)self + 0x2ec));
    *bitfield |= 4;
    *bitfield &= ~8;

    if (*(int*)(self + 0x98) <= 0xa000) {
        *(short*)(self + 0x69c) = 0;
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x26, 0, (Fix12i)0x1000, 0);
        *(unsigned char*)(self + 0x6e3) = 1;
    } else {
        int val = *(int*)(self + 0x98) / 16;
        *(short*)(self + 0x69c) = (short)val;
        if (*(short*)(self + 0x69c) >= 0x3000)
            *(short*)(self + 0x69c) = 0x3000;
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x27, 0x40000000, (Fix12i)0x1000, 0);
        *(unsigned char*)(self + 0x6e3) = 0;
    }

    void* base = *(void**)(self + 0x37c);
    void* vtable = *(void**)base;
    void* (*func)(void*) = *(void* (**)(void*))((char*)vtable + 8);
    void* res = func(base);

    *(int*)(self + 0x5c) = *(int*)res;
    *(int*)(self + 0x64) = *(int*)((char*)res + 8);
    *(int*)(self + 0x688) = *(int*)(self + 0x60) - *(int*)((char*)res + 4);
    *(int*)(self + 0x9c) = 0;
    *(int*)(self + 0x98) = 0;
    *(int*)(self + 0xa8) = 0;

    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(self + 0x6d9), 0xd,
                                           *(Vector3*)(self + 0x74));

    *(unsigned char*)(self + 0x6e5) = 0;
    *(short*)(self + 0x6b8) = 3;
    *(int*)(self + 0x2dc) = 0x5a000;
    *(unsigned char*)(self + 0x717) = 1;

    {
        char* b = *(char**)(self + 0x37c);
        if (*(int*)(b + 0x18) & 0x1000000) {
            if ((*(int*)(b + 8) >> 2) <= *(int*)(self + 0x688)) {
                if (func_ov002_020e3078(self, data_ov002_021106f4) == 0) {
                    *(short*)(self + 0x6a8) = 0xf;
                }
            }
        }
    }
    return 1;
}
