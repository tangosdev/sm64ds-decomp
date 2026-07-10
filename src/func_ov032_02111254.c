extern char* ClosestPlayer(char* c);
extern int Vec3_HorzDist(void* a, void* b);
extern int data_ov032_02113abc[];
extern int data_ov032_02113a7c[];
extern int data_0209f32c[];
int func_ov032_02111254(char *c) {
    char *pl = ClosestPlayer(c);
    int *s;
    int *t;
    int d;
    if (pl == 0 || *(unsigned short*)(c+0x400+0x2a) != 0)
        return 0;
    s = (int*)(((int)pl + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
    *(int*)(c+0x418) = s[0];
    *(int*)(c+0x41c) = s[1];
    *(int*)(c+0x420) = s[2];
    t = *(int**)(c+0x3b0);
    if (t != data_ov032_02113abc && t != data_ov032_02113a7c) {
        if (*(unsigned char*)(pl+0x706) == 0)
            return 0;
    }
    d = *(int*)(pl+0x644) - data_0209f32c[0];
    if (d < 0) d = -d;
    if (d < 0xb4000)
        return 0;
    if (t != data_ov032_02113abc && t != data_ov032_02113a7c) {
        if (Vec3_HorzDist(c+0x40c, c+0x418) > 0x4b0000)
            return 0;
    }
    return 1;
}
