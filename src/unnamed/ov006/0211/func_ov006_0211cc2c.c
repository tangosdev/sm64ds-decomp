void func_ov006_0211cc2c(unsigned char *self) {
    if (self[0x4c1f]) {
        self[0x4be3] = 3;
    } else {
        self[0x4be3] = 4;
    }
    *(int *)(self + 0x4bcc) = 0x80000;
    *(int *)(self + 0x4bd0) = 0x100000;
    *(int *)(self + 0x4bd8) = -0x1800;
    *(short *)(self + 0x4bdc) = 0;
    self[0x4be4] = 0;
    self[0x4be1] = 8;
    self[0x4be2] = 1;
    self[0x4c1b] = 0;
}
