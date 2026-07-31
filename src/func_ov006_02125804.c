extern void RenderOamBothScreens(int a, int b, int c, int d, int e, int f);
extern int data_ov006_02139c6c[];

void func_ov006_02125804(char *c)
{
    int i;
    char *p = c;
    for (i = 0; i < 0x20; i++) {
        if (*(unsigned char *)(p + 0xba34) != 0) {
            RenderOamBothScreens(
                data_ov006_02139c6c[*(int *)(p + 0xba30)],
                *(int *)(p + 0xba14) >> 0xc,
                ((((*(int *)(p + 0xba18) - *(int *)(p + 0xba1c)) - *(int *)(c + 0xab6c)) >> 0xc) - 0x110),
                -1, 0, 0);
        }
        p += 0x24;
    }
}
