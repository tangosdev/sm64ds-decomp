extern int data_ov001_020ad470;

void *func_ov001_020ab3c4(void *r0) {
    char *ptr = (char *)r0;
    *(int *)ptr = (int)&data_ov001_020ad470;
    *(int *)(ptr + 0xc) = 0;
    *(int *)(ptr + 0x10) = 0;

    *(unsigned char *)(int)(ptr + 0x1b) |= 4;
    return r0;
}
