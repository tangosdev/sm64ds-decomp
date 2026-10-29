extern int data_ov007_02103240[];
extern char *func_ov007_020c3df4(int, int);
extern void func_ov007_020c2024(char *);

char *func_ov007_020c2068(void) {
    char *p = func_ov007_020c3df4((int)data_ov007_02103240, 0x44);
    func_ov007_020c2024(p);
    return p;
}

void func_ov007_020c2024(char *c) {
    unsigned short *s = (unsigned short *)c;
    *(int *)(c + 0x20) = 0;
    *(int *)(c + 0x24) = 0;
    *(int *)(c + 0x28) = 0;
    s[0x19] = 0;
    s[0x18] = s[0x19];
    s[0x1b] = 0;
    s[0x1a] = s[0x1b];
    *(short *)(c + 0x3a) = 0;
    *(short *)(c + 0x38) = *(short *)(c + 0x3a);
    *(int *)(c + 0x2c) = 0;
    *(int *)(c + 0x40) = -1;
}
