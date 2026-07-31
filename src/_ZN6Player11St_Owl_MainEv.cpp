//cpp
extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* state);
extern void _Z14ApproachLinearRiii(int* p, int value, int speed);
extern int ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
extern void func_0201f32c(int a);
extern void func_0200d3f8(void* thiz, unsigned char playerID, void* ptr);
extern void func_ov002_020c9e40(char* c);
extern void func_0200d6b4(void* thiz, unsigned char playerID);
extern void func_ov002_020c9e18(char* c);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int d, unsigned int e);
extern void func_ov002_020bedd4(void* c);

extern unsigned char data_020a0e40;
extern unsigned char data_0209f49c[];
extern char data_0209f4a0[];
extern int data_ov002_021101b4[];
extern short data_02082214[];
extern char* data_0209f318;
extern unsigned char data_0209d660;
}

struct Player {
    int St_Owl_Main();
};

int Player::St_Owl_Main()
{
    char* c = (char*)this;
unsigned char st;

    *(int*)(c+0x684) = *(int*)(c+0x60);
    st = *(unsigned char*)(c+0x6e3);
    if (st != 2 && st != 3) {
        if (*(int*)(c+0x358) == 0 ||
            (*(unsigned short*)(data_0209f49c + data_020a0e40*0x18) & 2) == 0 ||
            (*(unsigned char*)(c+0x6e9) & 2)) {
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
            return 1;
        }
    }

    switch (st) {
    case 0:
        if (*(int*)(c+0x688) <= *(int*)(c+0x60)) {
            (*(unsigned char*)(((int)c + 0x6e3) & 0xFFFFFFFFFFFFFFFF))++;
            *(int*)(c+0x9c) = -0x2c00;
        }
        if (*(unsigned char*)(c+0x6de) != 0) {
            if (*(int*)(c+0xa8) < 0) {
                _Z14ApproachLinearRiii((int*)(c+0xa8), 0x3c000, 0x8000);
            } else {
                _Z14ApproachLinearRiii((int*)(c+0xa8), 0x3c000, 0x2000);
            }
        } else {
            *(int*)(c+0xa8) = 0x8000;
            *(unsigned char*)(c+0x6de) = 1;
            *(unsigned char*)(c+0x6df) = 0;
        }
        ApproachAngle((short*)(c+0x8e), *(short*)(c+0x69c), 0x10, 0x100, 0);
        break;

    case 1:
    case 4: {
        int x = *(int*)(c+0x358);
        short ang;
        int i;
        short mag = 10;
        x = *(int*)(x + 0x364);
        x = (unsigned short)((unsigned int)x >> 12);
        x += 0xb;
        ang = x * 0xccc;
        i = (unsigned short)ang >> 4;
        *(int*)(c+0xa8) = data_02082214[i*2+1] * mag;
        *(int*)(c+0x98) = 0x14000;
        if (*(short*)(data_0209f4a0 + data_020a0e40*0x18) != 0) {
            ApproachAngle((short*)(c+0x8e), *(short*)(c+0x6d2), 0x20, 0x100, 0);
        }
        if (*(unsigned char*)(c+0x6e3) == 1) {
            if (*(int*)(c+0x688) - 0xed8000 > *(int*)(c+0x60)) {
                *(unsigned char*)(c+0x724) = 1;
                func_0201f32c(0xa3);
                func_0200d3f8(data_0209f318, *(unsigned char*)(c+0x6d8), 0);
                *(int*)(c+0x9c) = 0;
                func_ov002_020c9e40(c);
                *(unsigned char*)(c+0x6e3) = 2;
            }
        } else {
            if (*(unsigned short*)(c+0x6a4) == 0) {
                _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
                return 1;
            }
        }
        break;
    }

    case 2:
        *(int*)(c+0x98) = 0;
        *(int*)(c+0xa8) = *(int*)(c+0x98);
        if (data_0209d660 == 0) {
            *(unsigned char*)(c+0x6e3) = 3;
            func_0200d6b4(data_0209f318, *(unsigned char*)(c+0x6d8));
        }
        break;

    case 3:
        if ((*(int*)(data_0209f318 + 0x154) & 0x8000) == 0) {
            *(unsigned char*)(c+0x6e3) = 4;
            *(unsigned short*)(c+0x6a4) = 0x3c;
            func_ov002_020c9e18(c);
            *(unsigned char*)(c+0x724) = 0;
        }
        break;
    }

    if (_ZN6Player12FinishedAnimEv(c)) {
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x58, 0, 0x1000, 0);
    }
    *(short*)(c+0x94) = *(short*)(c+0x8e);
    func_ov002_020bedd4(c);
    return 1;

}
