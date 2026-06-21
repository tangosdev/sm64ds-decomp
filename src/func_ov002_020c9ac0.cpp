//cpp
struct Player {
    void SetAnim(unsigned int, int, int, unsigned int);
};

extern "C" void func_ov002_020c9ac0(unsigned char *self)
{
    Player *p = (Player *)self;
    self[0x743] = 1;
    self[0x716] = 1;
    self[0x713] = 0;
    *(int *)(self + 0x9c) = 0;
    *(int *)(self + 0x98) = 0;
    *(int *)(self + 0xa8) = 0;
    self[0x6e3] = 0x10;
    p->SetAnim(0x93, 0x40000000, 0x1000, 0);
}
