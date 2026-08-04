extern char data_ov006_02140990[];
extern void func_ov006_020cf758(void *c);
void func_ov006_020d0ac0(void) {
    int i = 0;
    char *p = data_ov006_02140990;
    do {
        if (*(unsigned char*)(p + 0x328) != 0)
            func_ov006_020cf758(p);
        i++;
        p += 0x32c;
    } while (i < 4);
}
