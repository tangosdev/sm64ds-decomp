void func_02012718(void *a, int b);

void func_ov006_020fcd8c(char *this, int idx) {
    unsigned char *flag = (unsigned char*)(this + 0x4695) + idx * 0x38;
    char *e;
    if (*flag != 0) return;
    e = this + idx * 0x38;
    if ((*(int*)(e + 0x4664) >> 0xc) < -0xf2) return;
    *flag = *flag + 1;
    func_02012718((void*)0x184, *(int*)(e + 0x4660));
}
