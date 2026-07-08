//cpp
extern "C" unsigned char data_ov002_020ff240[];
extern "C" unsigned char PREV_RETURN_STATE;
extern "C" int _ZN6Player14ST_LEVEL_ENTERE;

struct State;
struct Player {
    char pad[0x6e3];
    unsigned char state;
};

extern void _ZN6Player4HealEi(Player* p, int amt);
extern void _ZN6Player11ChangeStateERNS_5StateE(Player* p, struct State* s);

extern "C" void func_ov002_020c7dd0(Player* p, int i) {
    unsigned char g;
    *(unsigned char*)((char*)p + 0x6e3) = data_ov002_020ff240[i];
    g = PREV_RETURN_STATE;
    if (g == 2) {
        unsigned char s = *(unsigned char*)((char*)p + 0x6e3);
        switch (s) {
        case 4:
            *(unsigned char*)((char*)p + 0x6e3) = 5;
            break;
        case 8:
            *(unsigned char*)((char*)p + 0x6e3) = 9;
            break;
        case 0xb:
            *(unsigned char*)((char*)p + 0x6e3) = 0xc;
            break;
        }
        _ZN6Player4HealEi(p, -0x880);
        _ZN6Player4HealEi(p, 0x100);
    } else if (g == 1) {
        if (*(unsigned char*)((char*)p + 0x6e3) == 4) {
            *(unsigned char*)((char*)p + 0x6e3) = 0xf;
        }
    }
    _ZN6Player11ChangeStateERNS_5StateE(p, (struct State*)&_ZN6Player14ST_LEVEL_ENTERE);
}
