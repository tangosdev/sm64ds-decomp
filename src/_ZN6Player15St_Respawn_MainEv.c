//cpp
// NONMATCHING: different op / idiom (div=36). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef unsigned short u16;

class Player;
class State;

extern "C" void FUN_02029980(void);
extern "C" void FUN_02029934(void);
extern "C" void _ZN6Player7SetAnimEji5Fix12IiEj(Player* self, unsigned int a, int b, int c, unsigned int d);
extern "C" int  _ZN6Player12FinishedAnimEv(Player* self);
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(Player* self, State& s);
extern "C" void func_ov002_020bedd4(char* self);
extern u8 CURR_PLAYER_ID;
extern State data_ov002_02110154;

extern "C" int _ZN6Player15St_Respawn_MainEv(Player* self)
{
    char* f = (char*)self;
    switch (*(u8*)(f + 0x6e3)) {
    case 0:
        switch (*(u16*)(f + 0x6a4)) {
        case 1:
            *(int*)(f + 0x9c) = -0x4000;
            if (*(u8*)(f + 0x6d8) == CURR_PLAYER_ID) {
                FUN_02029980();
                *(u16*)(f + 0x6a6) = 0x26;
            }
            break;
        case 0:
            if (*(u8*)(f + 0x6de) == 0) {
                u8* p = (u8*)(f + 0x6e3);
                _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x55, 0x40000000, 0x1000, 0);
                (*p)++;
            }
            break;
        }
        break;
    case 1:
        if (_ZN6Player12FinishedAnimEv(self)) {
            u8* p = (u8*)(f + 0x6e3);
            _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x47, 0, 0x1000, 0);
            (*p)++;
        }
        break;
    case 2:
        if (*(u16*)(f + 0x6a6) == 0) {
            if (*(u8*)(f + 0x6d8) == CURR_PLAYER_ID)
                FUN_02029934();
            _ZN6Player11ChangeStateERNS_5StateE(self, data_ov002_02110154);
        }
        break;
    }
    func_ov002_020bedd4(f);
    return 1;
}
