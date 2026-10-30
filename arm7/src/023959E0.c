extern int fn_023A4340(int, int, void *);
extern int fn_02395BE0(int, int);

int fn_023959E0(void)
{
    int v3;
    unsigned int v1;
    int v0;
    int r;
    *(volatile unsigned short *)0x4808160 = 0x100;
    v0 = 0x64;
    v1 = 0;
    v3 = 0;
    do {
        fn_023A4340(v0, 1, &v3);
        r = fn_02395BE0(v1, v3);
        ++v0;
        ++v1;
    } while (v1 < 0x69);
    return r;
}
