typedef short s16;
typedef unsigned short u16;

struct Mtx { int w[12]; };

extern void Matrix4x3_FromTranslation(struct Mtx* m, int x, int y, int z);
extern s16 data_02082214[];

void func_ov098_0213a984(char *c)
{
    u16 ang = *(u16*)(c + 0x8c);
    s16 ang2 = *(s16*)(c + 0x8e);
    char *p;
    int idx;

    Matrix4x3_FromTranslation((struct Mtx*)(c + 0xf0),
        *(int*)(c + 0x5c) >> 3,
        *(int*)(c + 0x60) >> 3,
        *(int*)(c + 0x64) >> 3);

    p = *(char**)(c + 0xe4);
    idx = ang >> 4;

    *(s16*)(p + 0x1c) = ang2;
    *(u16*)(p + 0x4e) = ang;
    *(int*)(p + 0x54) = 0;
    *(int*)(p + 0x58) = *(int*)(c + 0x194) + (*(int*)(c + 0x188) * data_02082214[idx * 2 + 1]) / 4096;
    *(int*)(p + 0x5c) = (*(int*)(c + 0x188) * data_02082214[idx * 2]) / 4096;
}
