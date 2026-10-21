extern void *data_020a6484;
int func_0205af4c(void) {
    int n = 0;
    void *p = data_020a6484;
    while (p) {
        p = *(void **)p;
        n++;
    }
    return n;
}

extern void *data_020a648c;
int func_0205af20(void) {
    int n = 0;
    void *p = data_020a648c;
    while (p) {
        p = *(void **)p;
        n++;
    }
    return n;
}

extern int func_0205af4c(void);
extern int func_0205af20(void);

int func_0205af00(void)
{
    int a = func_0205af4c();
    return (0x100 - a) - func_0205af20();
}
