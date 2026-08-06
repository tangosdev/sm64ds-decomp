#define M(p) ((long long)(int)(p) & 0xffffffffffffffffLL)
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_020fea70(char *o)
{
    int i;
    char *p;
    if (*(unsigned short *)(o + 0x5c1c) == 0) return;
    *(unsigned short *)(int)M(o + 0x5c1c) -= 1;
    if (*(short *)(o + 0x5c1c) > 0) return;
    *(unsigned short *)(o + 0x5c1c) = 0;
    p = o;
    for (i = 0; i < 0x30; i++, p += 0x38)
    {
        if (*(unsigned char *)(p + 0x4f0c) == 0)
        {
            *(unsigned char *)(o + i * 0x38 + 0x4000 + 0xf0c) = 1;
            *(unsigned char *)(o + i * 0x38 + 0x4000 + 0xf0e) = 1;
            *(unsigned char *)(o + i * 0x38 + 0x4000 + 0xf0d) = 0;
            *(int *)(o + i * 0x38 + 0x4000 + 0xefc) = 0;
            *(int *)(o + i * 0x38 + 0x4000 + 0xed8) = 0x80000;
            *(int *)(o + i * 0x38 + 0x4000 + 0xedc) = 0x28000;
            *(int *)(o + i * 0x38 + 0x4000 + 0xee8) = 0;
            *(int *)(o + i * 0x38 + 0x4000 + 0xeec) = 0;
            *(unsigned char *)(o + i * 0x38 + 0x4000 + 0xf0f) = 0;
            *(int *)(o + i * 0x38 + 0x4000 + 0xee0) = 0;
            *(int *)(o + i * 0x38 + 0x4000 + 0xee4) = 0;
            *(unsigned short *)(o + i * 0x38 + 0x4f0a) = 0;
            *(int *)(o + i * 0x38 + 0x4000 + 0xf00) = 0;
            *(int *)(o + i * 0x38 + 0x4000 + 0xf04) = 4;
            *(unsigned char *)(o + 0x5c2f) = (unsigned char)(i + 1);
            if (*(unsigned short *)(o + 0x5c20) != 0) {
                _ZN5Sound12PlayBank2_2DEj(0x18d);
                return;
            }
            *(unsigned short *)(int)M(o + 0x5c20) += 1;
            return;
        }
    }
}
