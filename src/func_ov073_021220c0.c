extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _Z14ApproachLinearRiii(int* p, int a, int b);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* a, void* clsn);
extern void func_ov073_021223a4(void* c, void* p);
extern short SINE_TABLE[];
extern int STAR_CAP_MIN_POS_Y;
extern int data_ov073_021234a0;

int func_ov073_021220c0(char* c)
{
    *(int*)(c + 0x328) = 0x400;
    if (DecIfAbove0_Short((unsigned short*)(c + 0x330)) != 0) {
        unsigned short* q = (unsigned short*)(((int)c + 0x332) & 0xFFFFFFFFFFFFFFFF);
        int t;
        *q += 0x800;
        t = *(int*)(c + 0x328);
        {
            int v = *(short*)((char*)(c + 0x300) + 0x32);
            *(short*)(c + 0x8c) = t + (int)(((long long)t * SINE_TABLE[(((int)((unsigned int)(v << 16) >> 16)) >> 4) * 2] + 0x800) >> 12);
        }
        _Z14ApproachLinearRiii((int*)(c + 0x328), 0, 0x40000);
        return 1;
    }
    *(short*)(c + 0x8c) = 0;
    *(int*)(c + 0x9c) = -0xa000;
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    if (*(unsigned char*)(c + 0x32c) != 0) goto end;
    {
        int v = STAR_CAP_MIN_POS_Y + 0x96000;
        if (v <= *(int*)(c + 0x60)) goto end;
        *(int*)(c + 0x60) = v;
        *(short*)(c + 0x330) = 0;
        *(int*)(c + 0x9c) = 0;
        *(int*)(c + 0xa8) = 0;
        func_ov073_021223a4(c, &data_ov073_021234a0);
    }
end:
    return 1;
}
