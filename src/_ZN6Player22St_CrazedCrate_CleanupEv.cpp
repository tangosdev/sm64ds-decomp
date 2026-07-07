//cpp
struct Player {
    char pad[0x2EC];
    int flags;
    char pad2[0x3F1];
    unsigned char byte_6E1;
    int St_CrazedCrate_Cleanup();
};

int Player::St_CrazedCrate_Cleanup() {
    *(int *)(((int)this + 0x2EC) & 0xFFFFFFFFFFFFFFFF) &= ~0x20;
    byte_6E1 = 0;
    return 1;
}
