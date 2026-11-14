extern char *fn_0238E688(void);
extern void fn_023928C4(void);
extern int fn_02382810(int a1, int a2, int a3, int a4);
extern void fn_02392B34(void);

int fn_023927EC(void) {
    char *p;
    int *obj;
    int inner;
    p = fn_0238E688();
    p += 0x1000;
    obj = *(int **)(p + 0x610);
    inner = *obj;
    if (*(unsigned int *)(inner + 0xD4) > 0x190)
        *(int *)(inner + 0x24) = 0;
    fn_023928C4();
    return fn_02382810((int)obj + 0xD0, 0xF0, (int)fn_02392B34, 0xF0);
}
