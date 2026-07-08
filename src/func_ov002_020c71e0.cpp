//cpp
struct State;
extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, struct State* s);
extern int func_ov002_020c6e14(char* c);
extern int func_ov002_020c44c4(char* c);
extern void func_ov002_020be008(char* c);
extern unsigned char VS_MODE_COUNTDOWN[];
extern unsigned char PREV_RETURN_STATE[];
extern struct State _ZN6Player7ST_SWIME;
extern struct State _ZN6Player7ST_WALKE;
void func_ov002_020c71e0(char* c) {
    *(unsigned char*)(c+0x711) = 1;
    if (VS_MODE_COUNTDOWN[0] != 0) return;
    if (*(unsigned short*)(c+0x6a6) == 0) {
        *(short*)(c+0x94) = *(short*)(c+0x8e);
        if (*(unsigned char*)(c+0x6e3) == 1) {
            _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_SWIME);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_WALKE);
            if (*(signed char*)(c+0x719) < 0 && PREV_RETURN_STATE[0] == 1) {
                if (func_ov002_020c6e14(c) != 0) return;
            }
            if (func_ov002_020c44c4(c) != 0) return;
        }
    }
    func_ov002_020be008(c);
}
}
