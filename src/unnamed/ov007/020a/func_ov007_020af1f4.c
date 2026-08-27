/* func_ov007_020af1f4 at 0x020af1f4 */

extern char *data_ov007_0210342c;
extern int func_ov007_020c1d8c(int a);

int func_ov007_020af1f4(void)
{
    char *r1 = data_ov007_0210342c;
    char *r2 = *(char **)(r1 + 0xc);

    if (*(short *)r2 != 7 || *(int *)(r1 + 0x20) != 0)
        *(short *)(r2 + 2) = 4;

    r1 = data_ov007_0210342c;
    if (*(short *)(*(char **)(r1 + 4) + 0xa) != 1)
        *(short *)(*(char **)(r1 + 8)) = 0;
    else
        *(short *)(*(char **)(r1 + 8)) = 1;

    return func_ov007_020c1d8c(0);
}
