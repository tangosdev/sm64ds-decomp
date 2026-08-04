extern char *func_020beb68;

extern int data_ov006_021424f8;
extern int data_ov006_021424f0;
extern int data_ov006_0213d568;
extern int data_ov006_0213d56c;
extern int data_ov006_0213d570;
extern int data_ov006_0213d5e0[8];
extern int data_ov006_021424fc;
extern int data_ov006_02142508;
extern short data_ov006_021424ec;
extern int data_ov006_021424f4;
extern int data_ov006_0213d574;
extern char *data_ov006_02142500;

extern void func_ov006_020f84a8(char *o, int n);
extern int func_ov006_020f7a00(void);
extern int func_ov006_020f7b90(void);

void func_ov006_020f7c10(char *c)
{
    int v;
    char *o;
    short i;
    short j;
    short off;
    char *p;
    char *q;
    char *n2;
    void *nx;

    o = func_020beb68;
    v = (o != 0) ? *(int *)(o + 0xb4) : 0;

    if (v < 5) {
        data_ov006_021424f8 = 6;
        data_ov006_021424f0 = 6;
        data_ov006_0213d568 = 5;
    } else if (v < 0xa) {
        data_ov006_021424f8 = 8;
        data_ov006_021424f0 = 7;
        data_ov006_0213d568 = 8;
    } else {
        data_ov006_021424f8 = 0xa;
        data_ov006_021424f0 = 8;
        data_ov006_0213d568 = 0xa;
    }

    data_ov006_0213d56c = data_ov006_021424f0 * data_ov006_021424f8;
    data_ov006_0213d570 = data_ov006_021424f0 * data_ov006_021424f8;

    do
    {
        for (i = 0; i < 8; i++)
        {
            if (i < data_ov006_021424f0)
                data_ov006_0213d5e0[i] = data_ov006_021424f8;
            else
                data_ov006_0213d5e0[i] = 0;
        }

        j = 0;
        p = c;
        for (; j < 0x50; j++)
        {
            func_ov006_020f84a8(p, j);
            if (j < data_ov006_0213d570)
            {
                p[0x2c] = (unsigned char)func_ov006_020f7a00();
            }
            p += 0x30;
        }

        off = 0;
        data_ov006_021424fc = 0;
        data_ov006_02142508 = 0;
        data_ov006_021424ec = 0;
        data_ov006_021424f4 = 0;
        data_ov006_0213d574 = 0;
        data_ov006_02142500 = c;

        if (data_ov006_0213d570 - 1 > 0)
        {
            q = c;
            do
            {
                n2 = c + (off + 1) * 0x30;
                nx = *(void **)(q + 8);
                *(void **)(q + 8) = n2;
                *(void **)(n2 + 4) = q;
                while (*(void **)(n2 + 8) != 0)
                {
                    n2 = *(void **)(n2 + 8);
                }
                *(void **)(n2 + 8) = nx;
                q += 0x30;
                off++;
            } while (off < data_ov006_0213d570 - 1);
        }
    } while (func_ov006_020f7b90() == 0);
}
