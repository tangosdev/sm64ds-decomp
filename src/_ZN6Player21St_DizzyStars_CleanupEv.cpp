//cpp
struct Player {
    int St_DizzyStars_Cleanup();
};
int Player::St_DizzyStars_Cleanup()
{
    char *self = (char *)this;
    *(unsigned int *)(((long long)(int)(self + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x80u;
    return 1;
}