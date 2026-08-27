extern int data_ov066_0211ae00;

int func_ov066_021175bc(char *r0) {
    int r3 = 0;
    *(int *)(r0 + 0x494) = r3;
    *(int *)(r0 + 0x498) = r3;
    char *r1 = r0 + 0x400;
    char *r2 = (char *)&data_ov066_0211ae00;
    *(short *)(r1 + 0xd0) = r3;
    *r2 = r3;
    *(int *)(r0 + 0x4a0) = r3;
    return 1;
}
