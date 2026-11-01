extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *);

int fn_023911C4(short a1, short a2)
{
    unsigned short *v4 = fn_0238DE4C();
    v4[0] = 12;
    v4[1] = 1;
    v4[2] = a1;
    v4[3] = a2;
    return fn_0238DEF4(v4);
}
