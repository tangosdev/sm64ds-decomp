extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern short data_02082214[];

void func_ov006_020f3a14(char *c, int i)
{
    int off = i * 0x14;
    int tx = i * 16 + 12;
    int dx, dy, idx;

    dx = tx - (*(int*)(c + 0x52c8 + off) >> 12);
    dy = -44 - (*(int*)(c + 0x52cc + off) >> 12);

    *(short*)(c + 0x52d4 + off) = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

    *(int*)(c + 0x52d0 + off) += 0x200;

    idx = *(unsigned short*)(c + 0x52d4 + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int*)(c + 0x52d0 + off);
        *(int*)(c + 0x52c8 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
    }

    idx = *(unsigned short*)(c + 0x52d4 + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int*)(c + 0x52d0 + off);
        *(int*)(c + 0x52cc + off) += (int)(((long long)tv * spd + 0x800) >> 12);
    }

    dx = tx - (*(int*)(c + 0x52c8 + off) >> 12);
    dy = -44 - (*(int*)(c + 0x52cc + off) >> 12);

    if (dx < -3) return;
    if (dx > 3) return;
    if (dy < -3) return;
    if (dy > 3) return;

    *(int*)(c + 0x52c8 + off) = tx << 12;
    *(int*)(c + 0x52cc + off) = -0x2c000;
    *(unsigned char*)(c + off + 0x5000 + 0x2da) = 2;
}
