//cpp
extern "C" {
int RandomIntInternal(int* seed);
int _ZNK9Animation13GetFrameCountEv(void* anim);
extern int data_0209e650;

void func_ov100_0214629c(char* c, int a1)
{
    *(int*)(c + 0x9c) = 0;
    *(int*)(c + 0xa0) = -0x1e000;
    unsigned int r = RandomIntInternal(&data_0209e650);
    int fc = _ZNK9Animation13GetFrameCountEv(c + 0x124);
    *(int*)(c + 0x12c) = (unsigned short)(r % (unsigned)fc) << 12;
    *(unsigned char*)(c + 0x159) = 0;
    *(int*)(c + 0x13c) = a1;
    if (*(unsigned char*)(c + 0x15c) != 0) {
        short* p = (short*)(((long long)(int)(c + 0x94)));
        *p = *p & 0x8000;
        unsigned char v = *(unsigned char*)(c + 0x15c);
        if (v == 3) {
            *p = *p + 0x2000;
        } else if (v == 2) {
            *p = *p + 0x6000;
        } else {
            *p = *p | 0x4000;
        }
        *(int*)(c + 0x98) = (((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x1fff) + 0x2000;
        *(unsigned char*)(c + 0x15a) = (unsigned char)((((unsigned)RandomIntInternal(&data_0209e650) >> 15) & 0x7f) + 0x3c);
        *(int*)(c + 0x14c) = 5;
    } else {
        *(int*)(c + 0x14c) = 3;
    }
}
}
