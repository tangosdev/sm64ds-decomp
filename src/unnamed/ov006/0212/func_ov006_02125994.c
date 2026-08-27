void func_ov006_02125994(char *c) {
    int i;
    for (i = 0; i < 0x20; i++) {
        *(int*)(c + 0xba14) = 0;
        *(int*)(c + 0xba18) = 0;
        *(int*)(c + 0xba1c) = 0;
        *(int*)(c + 0xba20) = 0;
        *(int*)(c + 0xba24) = 0;
        *(int*)(c + 0xba28) = 0;
        *(int*)(c + 0xba2c) = 0;
        *(unsigned char*)(c + 0xba34) = 0;
        *(int*)(c + 0xba30) = 0;
        c += 0x24;
    }
}
