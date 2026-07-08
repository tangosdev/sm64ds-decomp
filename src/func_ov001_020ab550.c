extern int GAME_SPEED_RELATED;

void func_ov001_020ab550(char *c)
{
    int v;
    int *p;

    v = *(int *)(c + 0xc);
    if (v <= 0) {
        return;
    }
    p = (int *)(((int)c + 0xc) & 0xFFFFFFFFFFFFFFFF);
    *p = *p - GAME_SPEED_RELATED;
    v = *(int *)(c + 0xc);
    if (v > 0) {
        return;
    }
    if (*(unsigned char *)(c + 0x10) != 0) {
        *(unsigned char *)(c + 0x10) = 0;
    } else {
        *(unsigned char *)(c + 0x10) = 1;
    }
    if (*(int *)(c + 0x14) >= 1) {
        *(unsigned char *)(c + 0x11) = 1;
    }
}