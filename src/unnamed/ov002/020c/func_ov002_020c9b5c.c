extern void func_ov002_020c9b10(char *self);

void func_ov002_020c9b5c(char *self) {
    *(unsigned char *)(self + 0x716) = 1;
    *(unsigned char *)(self + 0x713) = 0;
    *(int *)(self + 0x9c) = 0;
    func_ov002_020c9b10(self);
}
