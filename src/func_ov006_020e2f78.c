extern void func_ov006_020e1608(char *this);
extern void PlayBank2_2D(unsigned int);
extern void func_ov004_020adb1c(int self);
extern void func_ov006_020e2dbc(char *c);

typedef struct Elem {
    int a;
    int b;
    unsigned short cc;
    unsigned short d;
    unsigned char e;
    unsigned char f;
    unsigned char pad[2];
} Elem;

/* Layout matches ROM field offsets (arr @ 0x473c, st @ 0x4eac, f @ 0x4ee2, …). */
typedef struct Big {
    char pad0[0x473c];
    Elem arr[5];                    /* 0x473c .. 0x478b */
    char pad1[0x4eac - 0x478c];
    int st;                         /* 0x4eac */
    char pad2[0x4ee2 - 0x4eb0];
    unsigned short f;               /* 0x4ee2 */
    char pad3[0x4ee6 - 0x4ee4];
    unsigned char cnt;              /* 0x4ee6 */
    char pad4;                      /* 0x4ee7 */
    unsigned char flag;             /* 0x4ee8 */
} Big;

void func_ov006_020e2f78(char *c)
{
    Big *t = (Big *)c;
    int i;
    int sum;

    func_ov006_020e1608(c);

    if (t->f != 0)
    {
        *(unsigned short *)(((int)c + 0x4ee2) & 0xFFFFFFFFFFFFFFFF) -= 1;
        if ((short)t->f <= 0)
            t->f = 0;
        return;
    }

    t->st = 1;
    if (t->cnt >= 5)
    {
        t->st = 3;
        t->f = 0x80;
        t->flag = 1;
        sum = 0;
        PlayBank2_2D(0x1bc);
        for (i = 0; i < 5; i++)
            sum += t->arr[i].cc;
        func_ov004_020adb1c(sum);
    }

    for (i = 0; i < 5; i++)
    {
        t->arr[i].a = 0;
        t->arr[i].b = 0;
        t->arr[i].cc = 0;
        t->arr[i].d = 0;
        t->arr[i].f = 0;
        t->arr[i].e = 0;
    }

    func_ov006_020e2dbc(c);
}
