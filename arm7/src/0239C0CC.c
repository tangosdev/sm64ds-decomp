/* storage 0x0239C0CC runtime 0x027CE044 size 0x110 main ARM.
 * Pool: =0x380FFF4. Callees fn_02395334, fn_0239B89C. */

extern int fn_02395334(void *p);
extern void fn_0239B89C(void);

int fn_0239C0CC(unsigned short *a1, unsigned short *a2)
{
    int **v4 = (int **)0x380FFF4;
    unsigned int i;
    int v8;

    a2[1] = 0x12;
    if (*(unsigned short *)((char *)*v4 + 0x310) != 0x20)
        return 1;
    if (a1[9] > 3u)
        return 5;
    if (a1[10] > 0x3Fu)
        return 5;
    if (a1[11] == 0)
        return 5;
    if (a1[11] > 0x3E8u)
        return 5;
    for (i = 0; i < 0x10; ++i) {
        v8 = fn_02395334((char *)a1 + 0x18 + i);
        if (v8 == 0)
            break;
        if ((v8 & 0xF0) != 0)
            return 5;
        if (((1 << v8) & *(unsigned short *)((char *)*v4 + 0x2F0)) == 0)
            return 5;
    }
    if (i == 0)
        return 5;
    {
        int **v9 = (int **)0x380FFF4;
        *(unsigned short **)((char *)*v9 + 0x3D4) = a1;
        *(unsigned short **)((char *)*v9 + 0x3D8) = a2;
        a2[2] = 0x80;
        *(unsigned short *)((char *)*v9 + 0x3BC) = 0x80;
    }
    fn_0239B89C();
    return 0x80;
}
