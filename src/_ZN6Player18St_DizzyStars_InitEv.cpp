//cpp
extern "C" {
int _ZN6Player18St_DizzyStars_InitEv(char *c)
{
    *(short *)(c + 0x6a4) = 0x12c;
    *(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x80;
    return 1;
}
}
