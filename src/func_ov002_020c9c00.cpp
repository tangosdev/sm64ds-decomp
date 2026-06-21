//cpp
struct Player {
    void SetAnim(unsigned int, int, int, unsigned int);
};

extern "C" void func_ov002_020c9c00(Player *self) {
    if (*(unsigned char*)((char*)self + 0x6de)) {
        self->SetAnim(0x54, 0x40000000, 0x1000, 0);
    }
    *(unsigned char*)((char*)self + 0x6e3) = 0;
    *(int*)((char*)self + 0x98) = 0;
    *(int*)((char*)self + 0xa8) = 0;
}
