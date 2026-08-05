//cpp
struct State;
struct Player {
    void ChangeState(State&);
    unsigned int GetBodyModelID(unsigned int, bool) const;
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(void *, State&);

extern "C" void func_ov002_020cae10(char* c);
extern "C" int func_ov002_020cac60(char* c);
extern "C" void Player_AdvanceAnims(char* self);
extern signed char data_02092110;
extern State data_ov002_0211013c;
extern unsigned char data_020a0e40;
extern unsigned short data_0209f49e;

extern "C" int _ZN6Player18St_CameraZoom_MainEv(Player* thiz);
int _ZN6Player18St_CameraZoom_MainEv(Player* thiz) {
    char* self = (char*)thiz;
    if (data_02092110 < 0) {
        if ((unsigned short)(*(unsigned short*)(self + 0x6ce) & 4) == 0) {
            unsigned int id;
            _ZN6Player11ChangeStateERNS_5StateE(thiz, data_ov002_0211013c);
            id = thiz->GetBodyModelID(*(int*)(self + 8) & 0xff, 0);
            int q = (int)((*(volatile int*)(self + id * 4 + 0xdc) + 0x50));
            *(int*)(q + 8) = 0;
            *(short*)(self + 0x94) = *(short*)(self + 0x8e);
            return 1;
        }
        if (*(unsigned char*)(self + 0x6de)) {
            *(unsigned short*)(((int)self + 0x6ce)) &= ~4;
        }
        func_ov002_020cae10(self);
        if (*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18) & 0x8f03) {
            *(unsigned short*)(((int)self + 0x6ce)) &= ~4;
        }
        func_ov002_020cac60(self);
    }
    Player_AdvanceAnims(self);
    return 1;
}
