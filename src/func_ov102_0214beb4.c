extern short Vec3_HorzAngle(const void* a, const void* b);
extern int Vec3_Dist(const void* a, const void* b);
extern int RandomIntInternal(int* seed);
extern int RNG_STATE;

void func_ov102_0214beb4(char* c)
{
    *(int*)(c + 0x3dc) = 1;
    *(unsigned char*)(((int)c + 0x3f2) & 0xFFFFFFFFFFFFFFFF) += 1;
    char* o = *(char**)(c + 0x38c);
    if (o == 0) {
        *(short*)(c + 0x300 + 0xee) = Vec3_HorzAngle(c + 0x5c, c + 0x3c4);
        if (Vec3_Dist(c + 0x5c, c + 0x3c4) >= 0x500000) return;
        unsigned int r = RandomIntInternal(&RNG_STATE);
        *(short*)(((int)c + 0x3ee) & 0xFFFFFFFFFFFFFFFF) += (short)(r >> 0x10);
        return;
    }
    *(short*)(c + 0x300 + 0xee) = Vec3_HorzAngle(c + 0x5c, o + 0x5c);
}
