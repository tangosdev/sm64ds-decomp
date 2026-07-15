//cpp
extern "C" {
void _ZN8CapEnemy15RespawnIfHasCapEv(char *self);
int _ZN6Player15IsCollectingCapEv(char *p);
void _ZN5Actor15GivePlayerCoinsER6Playerhj(char *self, char *p, unsigned char a, unsigned int b);
void func_ov084_021296cc(char *self);
void _ZN5Actor11UntrackStarERa(char *self, signed char *p);
char *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, const void *pos, const void *rot, int e, int f);
void _ZN6Player20RegisterEggCoinCountEjbb(char *p, unsigned int a, int b, int c);
void func_ov084_02129498(char *self);
}

extern unsigned char data_0209f208;
extern unsigned char *data_0209f344;

struct Obj {
    virtual int f00();
    virtual int f01();
    virtual int f02();
    virtual int f03();
    virtual int f04();
    virtual int f05();
    virtual int f06();
    virtual int f07();
    virtual int f08();
    virtual int f09();
    virtual int f10();
    virtual int f11();
    virtual int f12();
    virtual int f13();
    virtual int f14();
    virtual int f15();
    virtual int f16();
    virtual int f17();
    virtual int GetState();
};

extern "C" void func_ov084_0212b344(char *self, char *player)
{
    Obj *o = (Obj *)self;
    int b5, b4;

    if ((*(unsigned char *)(self + 0x113) & 0xf) < 6 || *(unsigned char *)(self + 0x464) == 2) {
        *(int *)(self + 0x5c) = *(int *)(self + 0x41c);
        *(int *)(self + 0x60) = *(int *)(self + 0x420);
        *(int *)(self + 0x64) = *(int *)(self + 0x424);
        _ZN8CapEnemy15RespawnIfHasCapEv(self);
    }

    if (o->GetState() == 6) {
        if (_ZN6Player15IsCollectingCapEv(player)) {
            if (*(unsigned char *)(self + 0x108) == 1)
                _ZN5Actor15GivePlayerCoinsER6Playerhj(self, player, 1, 0);
            func_ov084_021296cc(self);
        } else {
            b5 = 0;
            b4 = b5;
            if (*(unsigned char *)(self + 0x108) == 1)
                b5 = 1;
            if (*(unsigned char *)(self + 0x464) == 1) {
                _ZN5Actor11UntrackStarERa(self, (signed char *)(self + 0x465));
                b4 = 1;
                _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb4, 0x50, self + 0x41c, 0, *(signed char *)(self + 0xcc), -1);
                *(int *)(self + 8) = *(int *)(self + 8) & 0xff0f;
            } else if (*(unsigned char *)(self + 0x464) == 2) {
                if (*(unsigned char *)(self + 0x466) == data_0209f344[data_0209f208]) {
                    _ZN5Actor11UntrackStarERa(self, (signed char *)(self + 0x465));
                    *(unsigned char *)(self + 0x464) = 3;
                    b4 = 1;
                }
            }
            _ZN6Player20RegisterEggCoinCountEjbb(player, b5, b4, 0);
        }
    } else if (o->GetState() == 4) {
        if (*(unsigned char *)(self + 0x108) == 1)
            _ZN5Actor15GivePlayerCoinsER6Playerhj(self, player, 1, 0);
    }

    func_ov084_02129498(self);
}
