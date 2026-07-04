void func_ov006_020da420(char *c);
void func_ov006_020da420(char *c){
    int i;
    char *p;
    short arr[6];
    short *a = (short *)(((int)arr) & 0xFFFFFFFFFFFFFFFF);
    a[0] = 0;
    a[1] = 0;
    a[2] = 0;
    a[3] = 0;
    a[4] = 0;
    a[5] = 0;
    p = c;
    for (i = 0; i < 5; i++) {
        arr[*(unsigned char*)(p + 0x2a)]++;
        p += 0x30;
    }
    for (i = 0; i < 5; i++) {
        if (arr[*(unsigned char*)(c + 0x2a)] == 1)
            *(unsigned char*)(c + 0x2b) = 4;
        c += 0x30;
    }
}
