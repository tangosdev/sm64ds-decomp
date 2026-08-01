#pragma opt_common_subs off
typedef unsigned char u8;

extern int func_020126e8(int a);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);

int func_ov006_020d25fc(char *p, int b, int dir)
{
    int col, row;
    u8 cur;
    u8 *pcur;

    col = *(int *)(p + b * 8 + 0x4660);
    row = *(int *)(p + b * 8 + 0x4664) + 0xc0;

    switch (dir) {
        case 0: col--; row--; break;
        case 1: row--; break;
        case 2: col++; row--; break;
        case 3: col--; break;
        case 4: col++; break;
        case 5: col--; row++; break;
        case 6: row++; break;
        case 7: col++; row++; break;
        default: return 0;
    }

    {
        u8 *t = (u8 *)(p + 0x4680);
        cur = t[b];
        pcur = t + b;
    }

    if (cur >= 2) {
        u8 mv = *(u8 *)(*(u8 **)(p + 0x4710) + col * 0x158 + row);
        if (mv != cur)
            return 0;

        *(int *)(p + b * 4 + 0x4684) = dir;
        *(int *)(p + b * 8 + 0x4660) = col;
        *(int *)(p + b * 8 + 0x4664) = row - 0xc0;
        return 1;
    } else {
        int q, r;

        if (*(u8 *)(*(u8 **)(p + 0x4710) + col * 0x158 + row) < 2)
            return 0;

        *(int *)(p + b * 4 + 0x4684) = dir;
        *pcur = *(u8 *)(*(u8 **)(p + 0x4710) + col * 0x158 + row);
        *(int *)(p + b * 8 + 0x4660) = col;
        *(int *)(p + b * 8 + 0x4664) = row - 0xc0;

        q = (*(int *)(p + b * 8 + 0x4664) + 0xd4) * 0x1f4 /
            (*(int *)(p + 0x4700) + 0xd4);
        r = func_020126e8(*(int *)(p + b * 8 + 0x4660) << 0xc);
        func_020126ac(0x1bf, 6, 0, q, r);
        return 1;
    }
}
