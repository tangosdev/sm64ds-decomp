typedef short s16;
typedef unsigned short u16;

void func_ov006_020e440c(char *c, int idx)
{
    int n = idx * 0x24;

    *(int *)(c + 0x48c0 + n) =
        *(int *)(c + 0x48c0 + n) + *(int *)(c + 0x48c8 + n);
    *(int *)(c + 0x48c4 + n) =
        *(int *)(c + 0x48c4 + n) + *(int *)(c + 0x48cc + n);

    if (*(u16 *)(c + 0x48d4 + n) != 0) {
        *(s16 *)(c + 0x48d4 + n) =
            (s16)(*(u16 *)(c + 0x48d4 + n) - 1);
        if (*(s16 *)(c + 0x48d4 + n) < 0)
            *(s16 *)(c + 0x48d4 + n) = 0;
        return;
    }

    if (*(int *)(c + 0x48c8 + n) > -0x400) {
        *(int *)(c + 0x48c8 + n) -= 0x20;
        if (*(int *)(c + 0x48c8 + n) <= -0x400)
            *(int *)(c + 0x48c8 + n) = 0x400;
    }

    if (*(u16 *)(c + 0x48d6 + n) != 0) {
        *(s16 *)(c + 0x48d6 + n) =
            (s16)(*(u16 *)(c + 0x48d6 + n) - 1);
        if (*(s16 *)(c + 0x48d6 + n) < 0)
            *(s16 *)(c + 0x48d6 + n) = 0;
        return;
    }

    *(unsigned char *)(c + 0x48de + n) = 3;
}
