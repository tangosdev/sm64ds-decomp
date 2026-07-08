extern int func_ov006_0210e120(int *self);

void func_ov006_0210e3e8(int *self) {
    unsigned char *b = (unsigned char *)self;
    self[4] = self[2];
    self[5] = self[3];
    if (self[0xd] > 0) {
        *(int *)(((long long)(int)(b + 0x34)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    }
    if (b[0x31] != 1) return;
    if (func_ov006_0210e120(self)) {
        if (self[0xd] == 0) b[0x31] = 0;
    } else {
        self[0xd] = 0xa;
    }
}
