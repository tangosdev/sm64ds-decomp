extern char *fn_0238E688(void);
extern int fn_02382810(int a1, int a2, int a3, int a4);
extern void fn_02392B34(void);
extern void fn_02392668(void);

void fn_023926D8(void) {
    char *p;
    int *obj;
    p = fn_0238E688();
    p += 0x1000;
    obj = *(int **)(p + 0x610);
    if (*(int *)(*obj + 0x10) == 1) {
        fn_02382810((int)obj + 0xD0, 0xD1, (int)fn_02392B34, 0xD1);
        fn_02392668();
    }
}
