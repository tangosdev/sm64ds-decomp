extern int func_ov002_020c5dec(void *this, int n);
int func_ov002_020c5cd8(char *c, int arg) {
    if (func_ov002_020c5dec(c, 6)) {
        *(int*)(c + 0x364) = arg;
        return 1;
    }
    return 0;
}
