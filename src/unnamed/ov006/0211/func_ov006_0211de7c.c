int func_ov006_0211de7c(char *c) {
    int cnt = 0;
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char*)(c + 0x4a70) != 0 && *(unsigned char*)(c + 0x4a73) != 2) {
            cnt++;
        }
        c += 0x14;
    }
    return cnt != 0;
}
