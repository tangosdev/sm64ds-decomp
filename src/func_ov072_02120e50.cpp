//cpp
struct Actor {
    char pad[0xc];
    unsigned short type;
    static Actor* FindWithID(unsigned int id);
};
struct Player {
    char pad[0xb0];
    int f0b0;
    char pad2[0x180-0xb4];
    int f0180;
    int f0184;
    char pad3[0x360-0x188];
    Actor* f0360;
    bool TryGrab(Actor& a);
};
extern "C" void func_ov072_02121d50(Player* p, int kind);
extern "C" int func_ov072_02120e50(Player* self)
{
    Actor* a;
    int isType;
    int flag;
    if (self->f0184 == 0) return 0;
    a = Actor::FindWithID(self->f0184);
    if (a == 0 || (isType = (a->type == 0xbf)) == 0) return 0;
    flag = ((self->f0b0 & 0x20000) != 0);
    if (flag) {
        self->f0360 = a;
        func_ov072_02121d50(self, 4);
    } else if ((self->f0180 & 0x1000) && ((Player*)a)->TryGrab(*(Actor*)self)) {
        self->f0360 = a;
        func_ov072_02121d50(self, 2);
    }
    return 1;
}
