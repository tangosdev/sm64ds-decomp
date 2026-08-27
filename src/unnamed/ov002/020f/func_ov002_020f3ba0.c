extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern short data_02082214[];

void func_ov002_020f3ba0(char *c, int i)
{
    int off = i * 0x4c;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;

    idx = *(unsigned short*)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int*)(c + 8 + off);
        *(int*)(c + 0 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
    }

    idx = *(unsigned short*)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int*)(c + 8 + off);
        *(int*)(c + 4 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
    }

    *(int*)(c + 8 + off) += 0x30;

    dx = 0xe0 - (*(int*)(c + 0 + off) >> 12);
    dy = 0x46 - (*(int*)(c + 4 + off) >> 12);

    if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
        *(unsigned char*)(c + 0x47 + off) += 1;
        *(unsigned short*)(c + 0x30 + off) = 0;
        *(unsigned short*)(c + 0x2e + off) = 0xc000;
        *(int*)(c + 0x10 + off) = 0x20000;
        *(int*)(c + 0 + off) = 0xe0000;
        *(int*)(c + 4 + off) = 0x46000;
        return;
    }

    target = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
    ang = *(unsigned short*)(c + 0x2e + off);
    if (target > ang) {
        *(unsigned short*)(c + 0x2e + off) += 0x80;
        if (target <= *(unsigned short*)(c + 0x2e + off))
            *(unsigned short*)(c + 0x2e + off) = target;
    } else {
        if (ang <= target)
            return;
        *(unsigned short*)(c + 0x2e + off) -= 0x80;
        if (*(unsigned short*)(c + 0x2e + off) <= target)
            *(unsigned short*)(c + 0x2e + off) = target;
    }
}
