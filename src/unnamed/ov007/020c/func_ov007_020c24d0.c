extern void func_ov007_020c2d44(void *s, int i);

struct E12 { int a, b, c; };

struct S {
    int state;          // +0
    int flag;           // +4
    unsigned short count; // +8
    char pad[0x18 - 0xa];
    int *p18;           // +0x18
    unsigned short *p1c; // +0x1c
    int *p20;           // +0x20
    int *p24;           // +0x24
    int *p28;           // +0x28
    struct E12 *p2c;    // +0x2c
};

void func_ov007_020c24d0(struct S *s, int arg1)
{
    int *p18;
    int *p24;
    int *p28;
    unsigned short *p1c;
    int i;

    if (s->count == 0)
        return;
    if (arg1 >= s->count)
        return;
    (*(unsigned short *)(((int)s + 8)))--;
    if (arg1 == s->count)
        return;

    i = (int)(((long long)arg1));
    while (i < s->count) {
        switch (s->state) {
        case 0:
            p24 = s->p24;
            p24[i] = p24[i + 1];
            p28 = s->p28;
            p28[i] = p28[i + 1];
            p1c = s->p1c;
            p1c[i] = p1c[i + 1];
            break;
        case 1:
            s->p2c[i] = s->p2c[i + 1];
            break;
        }
        p18 = s->p18;
        p18[i] = p18[i + 1];
        if (s->flag & 1)
            s->p20[i] = s->p20[i + 1];
        i++;
    }

    switch (s->state) {
    case 0:
        func_ov007_020c2d44(s, arg1);
        break;
    case 1:
        break;
    }
}
