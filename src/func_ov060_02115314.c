extern int RandomIntInternal(int* seed);
extern int RNG_STATE;

void func_ov060_02115314(char* c)
{
    if (*(unsigned char*)(c + 0x415) == 0) {
        if (*(int*)(c + 0x418) & 2) {
            if (*(int*)(c + 0x3ec) < 0x5dc000)
                *(int*)(c + 0x40c) = 0xf;
            else
                *(int*)(c + 0x40c) = 0x11;
        } else {
            *(int*)(c + 0x40c) = 0xe;
        }
        unsigned char* q = (unsigned char*)(((int)c + 0x415) & 0xFFFFFFFFFFFFFFFF);
        *q = *q + 1;
        *(int*)(c + 0x3f8) = 0x1000;
    } else {
        *(unsigned char*)(c + 0x415) = 0;
        if (*(unsigned char*)(c + 0x429) == 0) {
            unsigned int v = (unsigned int)RandomIntInternal(&RNG_STATE) >> 0x10;
            if (v % 10 == 0)
                *(int*)(c + 0x40c) = 3;
            else
                *(int*)(c + 0x40c) = 0xe;
        } else {
            *(unsigned char*)(c + 0x429) = 0;
            *(int*)(c + 0x40c) = 0xe;
        }
    }
}
