//cpp
extern "C" {
extern void func_ov002_020bdb50(char *c, int arg);
}

struct Player {
    int St_InYoshiMouth_Init();
};

int Player::St_InYoshiMouth_Init()
{
    char * c = (char *)this;
unsigned int r3;
    char *slot;
    unsigned int r1;

    func_ov002_020bdb50(c, 0);
    r3 = 0;
    *(unsigned char *)(c + 0x6e5) = (unsigned char)r3;
    *(unsigned char *)(c + 0x6e6) = (unsigned char)r3;
    slot = (char *)(((long long)(int)(c + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL);
    r1 = *(unsigned int *)slot;
    r1 |= 2u;
    *(unsigned int *)slot = r1;
    *(unsigned char *)(c + 0x713) = (unsigned char)r3;
    return 1;

}
