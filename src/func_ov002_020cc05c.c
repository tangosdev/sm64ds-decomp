typedef unsigned char u8;
typedef short s16;
typedef long long s64;
extern u8 data_020a0e40;
extern s16 data_0209f4a2[];
void func_ov002_020cc05c(char *self)
{
    int idx = data_020a0e40 * 0x18;
    s16 curval = *(s16*)(((int)self + 0x69c) & 0xFFFFFFFFFFFFFFFFull);
    *(s16*)(((int)self + 0x69c) & 0xFFFFFFFFFFFFFFFFull) = curval + (s16)(int)(((s64)*(s16*)((char*)data_0209f4a2 + idx) * 0x80 + 0x800) >> 12);
    {
        s16 tv2 = *(s16*)((char*)data_0209f4a2 + idx);
    {
        s16 av = tv2;
        if (av < 0) av = (s16)(-av);
        {
            int mag32 = (int)(((s64)av * 0x480 + 0x800) >> 12);
            s16 mag = (s16)mag32;
            s16 nmag = (s16)(-mag);
            s16 cur2 = *(s16*)(self + 0x600 + 0x9c);
            s16 result;
            if (cur2 < nmag) result = nmag;
            else {
                if (cur2 <= mag) mag = cur2;
                result = mag;
            }
            *(s16*)(self + 0x600 + 0x9c) = result;
        }
    }
}
}
