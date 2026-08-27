extern int func_0205df40(void *buf, int a, int b);
extern int func_0205dc60(void *t);

int func_0205dc0c(int a, int b) {
    char buf[0x24];
    if (func_0205df40(buf, a, b)) {
        if (func_0205dc60(buf)) goto ret1;
    }
    return 0;
ret1:
    return 1;
}
