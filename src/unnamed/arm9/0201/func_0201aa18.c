extern int func_0201aad4(void *p);

void *func_0201aa18(char *c) {
    func_0201aad4(c);
    *(int *)(c + 0x30) = 0;
    *(int *)(c + 0x34) = 0;
    *(unsigned char *)(c + 0x40) = 1;
    *(int *)(c + 0x2c) = 0;
    *(int *)(c + 0x38) = 0;
    *(int *)(c + 0x3c) = 0;
    return c;
}
