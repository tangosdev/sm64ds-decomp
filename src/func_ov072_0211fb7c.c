void func_0203568c(int *p, int v);
void func_02035684(int *p, int v);
void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void *thiz, int a, int b, int c, int d);

int func_ov072_0211fb7c(char *c) {
    int *p;
    *(int*)(c + 0x9c) = 0;
    *(int*)(c + 0xa0) = 0;
    *(int*)(c + 0x80) = 0x800;
    *(int*)(c + 0x84) = 0x800;
    *(int*)(c + 0x88) = 0x800;
    *(int*)(c + 0x398) = (int)(((long long)(*(int*)(c + 0x80)) * 0x82000 + 0x800) >> 12);
    *(int*)(c + 0x150) = *(int*)(c + 0x398);
    *(int*)(c + 0x154) = *(int*)(c + 0x398) << 1;
    func_0203568c((int*)(c + 0x180), *(int*)(c + 0x398));
    func_02035684((int*)(c + 0x180), *(int*)(c + 0x398));
    _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(c, *(int*)(c + 0x398), *(int*)(c + 0x398), 0x1000000, 0x1000000);
    p = (int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
    *p = *p | 1;
    *(int*)(c + 0x394) = 0;
    return 1;
}
