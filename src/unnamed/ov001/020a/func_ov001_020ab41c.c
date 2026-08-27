extern void func_ov001_020ab3f0(void *r0);

void func_ov001_020ab41c(void *c) {
    if (*(unsigned char *)((char *)c + 0x10) == 0) {
        *(unsigned char *)((char *)c + 0x10) = 1;
        return;
    }
    func_ov001_020ab3f0(c);
}
