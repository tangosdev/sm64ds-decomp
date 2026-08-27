typedef int Fix12i;
extern unsigned char data_0209f250;
extern void* data_0209f318;
extern short GetAngleToCamera(int i);

void func_ov002_020cae10(char* c)
{
    int r4 = 0x4000;
    int ang;
    int r8;
    if (*(unsigned char*)(c + 0x6d8) == data_0209f250) {
        r4 -= *(short*)((char*)*(void**)&data_0209f318 + 0x17e);
        r4 = (short)r4;
    }
    ang = (short)((*(short*)(c + 0x8e) - GetAngleToCamera(*(unsigned char*)(c + 0x6d8))) + 0x8000);
    r8 = ang;
    if (r8 < (int)0xffffd556) r8 = (int)0xffffd556;
    if (r8 > 0x2aaa) r8 = 0x2aaa;
    *(short*)(c + 0x764) = (short)(int)(((long long)r8 * 0xc00 + 0x800) >> 12);
    *(short*)(c + 0x766) = (short)(int)(((long long)r4 * 0xc00 + 0x800) >> 12);
    *(unsigned char*)(c + 0x742) = 2;
}
