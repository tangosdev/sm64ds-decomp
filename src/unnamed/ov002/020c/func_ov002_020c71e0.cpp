//cpp
struct State;
extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, struct State* s);
extern int func_ov002_020c6e14(char* c);
extern int func_ov002_020c44c4(char* c);
extern void func_ov002_020be008(char* c);
extern unsigned char data_0209f2bc[];
extern unsigned char data_0209f2fc[];
extern struct State data_ov002_0211067c;
extern struct State data_ov002_0211013c;
void func_ov002_020c71e0(char* c) {
    *(unsigned char*)(c+0x711) = 1;
    if (data_0209f2bc[0] != 0) return;
    if (*(unsigned short*)(c+0x6a6) == 0) {
        *(short*)(c+0x94) = *(short*)(c+0x8e);
        if (*(unsigned char*)(c+0x6e3) == 1) {
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211067c);
        } else {
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
            if (*(signed char*)(c+0x719) < 0 && data_0209f2fc[0] == 1) {
                if (func_ov002_020c6e14(c) != 0) return;
            }
            if (func_ov002_020c44c4(c) != 0) return;
        }
    }
    func_ov002_020be008(c);
}
}
