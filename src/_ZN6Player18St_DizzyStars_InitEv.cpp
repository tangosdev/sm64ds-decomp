//cpp
struct Player {
    int St_DizzyStars_Init();
};

int Player::St_DizzyStars_Init()
{
    char * c = (char *)this;
*(short *)(c + 0x6a4) = 0x12c;
    *(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x80;
    return 1;

}
