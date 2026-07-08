struct Pair { int a, b; };
extern struct Pair data_ov006_0213e2f8;

void func_ov006_021082c4(char *p)
{
    *(int *)(((long long)(int)(p + 0xb8)) & 0xFFFFFFFFFFFFFFFFLL) += 0x140;
    *(short *)(p + 0xc4) = -*(short *)(p + 0xc2);
    *(struct Pair *)p = data_ov006_0213e2f8;
}
