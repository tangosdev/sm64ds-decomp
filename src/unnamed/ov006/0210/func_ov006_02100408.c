void func_ov006_02100408(char *c, int a2, int a3)
{
    int i = 0;
    char *p = c;
    do {
        if (*(unsigned char *)(p + 0x5336) == 0) {
            char *e = c + i * 0x18;
            *(int *)(e + 0x5320) = a2;
            *(int *)(e + 0x5324) = a3;
            *(unsigned char *)(e + 0x5336) = 1;
            *(unsigned short *)(e + 0x5330) = 0;
            *(int *)(e + 0x5328) = 0;
            *(int *)(e + 0x532c) = 0;
            *(unsigned char *)(e + 0x5334) = 1;
            *(unsigned char *)(e + 0x5335) = 0;
            return;
        }
        i++;
        p += 0x18;
    } while (i < 0x10);
}
