typedef short s16;
typedef unsigned short u16;

void func_ov006_020e0edc(char *c, int idx)
{
    char *b = c;
    int off = idx * 0x24;

    *(int *)(b + 0x478c + off) += *(int *)(b + 0x4794 + off);
    *(int *)(b + 0x4790 + off) +=
        *(int *)((char *)b + off + 0x4000 + 0x798);

    u16 *p1 = (u16 *)(b + 0x47a0 + off);
    if (*p1 != 0) {
        *p1 = *p1 - 1;
        s16 v = *(s16 *)p1;
        if (v < 0) {
            *p1 = 0;
        }
        return;
    }

    int *p2 = (int *)(b + 0x4794 + off);
    if (*p2 > -0x400) {
        *p2 -= 0x20;
        if (*p2 <= -0x400) {
            *p2 = 0x400;
        }
    }

    u16 *p3 = (u16 *)(b + 0x47a2 + off);
    if (*p3 != 0) {
        *p3 = *p3 - 1;
        s16 v = *(s16 *)p3;
        if (v < 0) {
            *p3 = 0;
        }
        return;
    }

    *(unsigned char *)(b + off + 0x4000 + 0x7aa) = 3;
}
