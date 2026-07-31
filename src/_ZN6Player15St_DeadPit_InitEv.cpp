//cpp
extern "C" {
typedef unsigned short u16;
typedef unsigned char u8;

extern u8 data_0209f2d8;
extern u8 data_0209f250;

int func_ov002_020dab14(char* c);
void func_ov002_020d5cec(char* c);
void func_ov002_020c9e40(char* c);
void HitDeathPlane(int arg);
void FUN_020299f4(void);
void _ZN6Player7SetAnimEji5Fix12IiEj(char* c, unsigned int a, int b, int f, unsigned int g);
void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, char* v);
}

struct Player {
    int St_DeadPit_Init();
};

int Player::St_DeadPit_Init()
{
    char* c = (char*)this;
func_ov002_020dab14(c);
    if (*(char**)(c + 0x360) != 0) {
        int b = (*(u16*)(*(char**)(c + 0x360) + 0xc) == 0xbf);
        if (b) {
            func_ov002_020d5cec(*(char**)(c + 0x360));
            *(u16*)(((int)c + 0x6ce) & 0xFFFFFFFFFFFFFFFF) &= ~2;
            *(int*)(*(char**)(c + 0x360) + 0xd0) = 0;
        }
        *(u8*)(c + 0x714) = 0;
        *(int*)(((int)*(char**)(c + 0x360) + 0xb0) & 0xFFFFFFFFFFFFFFFF) |= 0x80000;
        *(int*)(((int)*(char**)(c + 0x360) + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~0x40000;
        *(int*)(((int)*(char**)(c + 0x360) + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~0x20000;
        *(char**)(c + 0x360) = 0;
    }
    func_ov002_020c9e40(c);
    {
        int t = 0;
        *(u8*)(c + 0x713) = t;
        *(u8*)(c + 0x6e5) = t;
        switch (*(u8*)(c + 0x6e3)) {
        case 0:
            break;
        case 1:
            if (data_0209f2d8 == 1) t = 1;
            if (t == 0) {
                int a = 2;
                if (*(int*)(c + 0x664) == 5) a = 0;
                HitDeathPlane(a);
            } else {
                if (*(u8*)(c + 0x6d8) == data_0209f250) FUN_020299f4();
                *(u16*)(c + 0x6a6) = 0x1e;
            }
            *(u8*)(c + 0x716) = 1;
            break;
        case 2:
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 7, 0x40000000, 0x1000, t);
            *(int*)(c + 0xa8) = 0xa000;
            *(int*)(((int)c + 0x98) & 0xFFFFFFFFFFFFFFFF) >>= 1;
            *(u8*)(c + 0x6de) = 1;
            *(u8*)(c + 0x6df) = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 0xb, c + 0x74);
            break;
        case 3:
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x82, 0x40000000, 0x1000, t);
            *(int*)(c + 0xa8) = 0;
            *(int*)(c + 0x98) = 0;
            *(u8*)(c + 0x70c) = 0;
            break;
        case 4:
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 2, 0x40000000, 0x1000, t);
            *(u16*)(c + 0x6a4) = 0x64;
            *(int*)(c + 0xa8) = 0;
            *(int*)(c + 0x98) = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 0xb, c + 0x74);
            break;
        case 5:
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 6, 0x40000000, 0x1000, t);
            *(int*)(c + 0xa8) = 0;
            *(int*)(c + 0x98) = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 0xb, c + 0x74);
            break;
        case 6:
            *(u16*)(c + 0x6a4) = 0x3c;
            *(int*)(c + 0xa8) = t;
            *(int*)(c + 0x98) = t;
            *(int*)(c + 0x9c) = t;
            break;
        case 7:
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x84, 0x40000000, 0x1000, t);
            *(int*)(c + 0x9c) = 0;
            *(int*)(c + 0xa0) = -0x1800;
            *(int*)(c + 0xa8) = 0;
            *(int*)(c + 0x98) = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 0x34, c + 0x74);
            break;
        case 8:
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x83, 0x40000000, 0x1000, t);
            *(int*)(c + 0x9c) = 0;
            *(int*)(c + 0xa0) = -0x1800;
            break;
        case 9:
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 0x36, c + 0x74);
            break;
        }
    }
    return 1;

}
