//cpp
#include "types.h"
struct State;
struct Player {
    int St_WindCarry_Main();
    void ChangeState(State& st);
    int SetAnim(unsigned int a, int b, Fix12i c, unsigned int d);
    int FinishedAnim();
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(void *, State& st);
extern "C" int _ZN6Player7SetAnimEji5Fix12IiEj(void *, unsigned int a, int b, Fix12i c, unsigned int d);


extern "C" void func_ov002_020c2f64(void* c);
extern "C" void func_ov002_020e28d4(void* c, int a, int b);
extern "C" void Player_AdvanceAnims(char* self);
extern "C" int __aeabi_idiv(int a, int b);

extern State data_ov002_021105bc;

int Player::St_WindCarry_Main()
{
    char* c = (char*)this;

    if (*(u8*)(c + 0x6de) == 0) {
        func_ov002_020c2f64(c);
        _ZN6Player11ChangeStateERNS_5StateE(this, data_ov002_021105bc);
        _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x43, 0x40000000, 0x1000, 0);
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
                int* p = (int*)(((int)c + 0xa8));
                *p = *p + (speed >> 3);
                if (*(int*)(c + 0xa8) > speed) *(int*)(c + 0xa8) = speed;
            }
        }
    }

    if (*(u8*)(c + 0x6e3) == 0 && FinishedAnim() != 0) {
        _ZN6Player7SetAnimEji5Fix12IiEj(this, 0x73, 0, 0x1000, 0);
        {
            u8* q = (u8*)(((int)c + 0x6e3));
            *q = *q + 1;
        }
    }
    Player_AdvanceAnims(c);
    return 1;
}
