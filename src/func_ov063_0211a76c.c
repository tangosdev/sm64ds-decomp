extern short SINE_TABLE[];
extern short data_ov063_0211e7e0[];

void func_ov063_0211a76c(char* c, int cond, int val)
{
    int ip = *(unsigned short*)(c + 0x100) + 1;

    ip = ((ip << 0x1b) >> 0x10);
    ip = (unsigned short)ip;
    ip = ip >> 4;
    ip = (ip << 1) + 1;

    *(int*)(c + 0x98) = val;
    *(int*)(c + 0xa8) = SINE_TABLE[ip];
    *(short*)(c + 0x94) = *(short*)(c + 0x5b6);
    if (cond == 0) return;

    {
        short* p8e = (short*)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFF);
        *p8e = *p8e + data_ov063_0211e7e0[*(unsigned short*)(c + 0x100)];
        {
            short* p90 = (short*)(((int)c + 0x90) & 0xFFFFFFFFFFFFFFFF);
            *p90 = *p90 + data_ov063_0211e7e0[*(unsigned short*)(c + 0x100)];
        }
    }
}
