extern int RandomIntInternal(int* seed);
extern int func_02012694(int a, void* b, void* c, int d);
extern int RNG_STATE;

int func_ov065_021175b0(char* c)
{
    unsigned int r;
    r = (unsigned int)RandomIntInternal(&RNG_STATE);
    *(short*)(c + 0x434) = ((r >> 8) & 0xf) << 0xc;
    r = (unsigned int)RandomIntInternal(&RNG_STATE);
    *(short*)(c + 0x100) = ((r >> 8) & 0x1f) + 0x32;
    func_02012694(0xf9, c + 0x74, c + 0x100, ((r >> 8) & 0x1f) + 0x32);
    *(int*)(c + 0x35c) = 0x1000;
    (*(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF)) &= ~1;
    return 1;
}
