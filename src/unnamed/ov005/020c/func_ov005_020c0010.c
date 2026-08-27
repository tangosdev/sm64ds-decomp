extern int data_ov005_020c24dc[];

int func_ov005_020c0010(int a, int i) {
    int offset = i * 0x34;
    return (*(unsigned int*)((char*)data_ov005_020c24dc + offset) >> 8) & 0xff;
}
