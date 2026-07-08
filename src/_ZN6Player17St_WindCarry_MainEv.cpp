//cpp
typedef int Fix12;
typedef unsigned char u8;

struct State;
struct Player {
    int St_WindCarry_Main();
    void ChangeState(State& st);
    int SetAnim(unsigned int a, int b, Fix12 c, unsigned int d);
    int FinishedAnim();
};

extern "C" void func_ov002_020c2f64(void* c);
extern "C" void func_ov002_020e28d4(void* c, int a, int b);
extern "C" void func_ov002_020bedd4(char* self);
extern "C" int __aeabi_idiv(int a, int b);

extern State _ZN6Player7ST_DIVEE;

int Player::St_WindCarry_Main()
{
    char* c = (char*)this;

    if (*(u8*)(c + 0x6de) == 0) {
        func_ov002_020c2f64(c);
        ChangeState(_ZN6Player7ST_DIVEE);
        SetAnim(0x43, 0x40000000, 0x1000, 0);
        *(int*)(c + 0xa8) = 0;
        return 1;
    }

    func_ov002_020e28d4(c, 0x1800, 0x800);
    {
        int val = *(int*)(c + 0x60) + 0x514000;
        if (*(int*)(c + 0x664) == 0x13 && val > (int)0xfdecc000 && val < 0x898000) {
            int speed;
            if (val < 0)
                speed = 0x32000;
            else
                speed = __aeabi_idiv(0x2710000, (val + 0xc8000) >> 12);
            if (*(int*)(c + 0xa8) < speed) {
                int* p = (int*)(((int)c + 0xa8) & 0xFFFFFFFFFFFFFFFF);
                *p = *p + (speed >> 3);
                if (*(int*)(c + 0xa8) > speed) *(int*)(c + 0xa8) = speed;
            }
        }
    }

    if (*(u8*)(c + 0x6e3) == 0 && FinishedAnim() != 0) {
        SetAnim(0x73, 0, 0x1000, 0);
        {
            u8* q = (u8*)(((int)c + 0x6e3) & 0xFFFFFFFFFFFFFFFF);
            *q = *q + 1;
        }
    }
    func_ov002_020bedd4(c);
    return 1;
}
