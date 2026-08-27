typedef struct Vector3 {
    int x;
    int y;
    int z;
} Vector3;

extern void func_02012694(unsigned int id, const Vector3 *v);
extern void func_ov060_02111cc0(char *c, int idx, int a);
extern int Bowser_IsAnimAtLastFrame(char *o);
extern short data_02082214[];

void func_ov060_02112724(char *c)
{
    int v;
    short a;
    int i;
    int j;
    Vector3 *s;

    *(unsigned int *)(c + 0x418) &= ~0x20000;

    switch (*(unsigned char *)(c + 0x425)) {
    case 0:
        *(unsigned int *)(c + 0x378) |= 1;
        func_02012694(0xb2, (const Vector3 *)(c + 0x74));
        *(int *)(c + 0x40c) = 1;
        func_ov060_02111cc0(c, 0xa, 0);
        *(int *)(c + 0x3f8) = 0x1000;
        (*(unsigned char *)(c + 0x425))++;
        break;
    case 1:
        if (Bowser_IsAnimAtLastFrame(c)) {
            func_ov060_02111cc0(c, 9, 0);
            (*(unsigned char *)(c + 0x425))++;
        }
        break;
    case 2:
        break;
    }

    *(int *)(c + 0x3f0) = *(short *)(*(char **)(c + 0x3a4) + 0x69c);
    v = *(int *)(c + 0x3f0);
    a = *(short *)(*(char **)(c + 0x3a4) + 0x8e);
    if (v < 0) {
        v = -v;
    }
    *(short *)(c + 0x8c) = -v;
    *(short *)(c + 0x8e) = a;



    s = (Vector3 *)(*(char **)(c + 0x3a4) + 0x5c);
    *(int *)(c + 0x5c) = s->x;
    *(int *)(c + 0x60) = s->y;
    *(int *)(c + 0x64) = s->z;

    i = ((unsigned short)a >> 4) * 2;

    *(int *)(c + 0x5c) += data_02082214[i] * 0xa0;
    j = (*(unsigned short *)(c + 0x8c) >> 4) * 2;
    *(int *)(c + 0x60) += 0x18000 - data_02082214[j] * 0xa0;
    *(int *)(c + 0x64) += data_02082214[i + 1] * 0xa0;
}
