extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char*, void*, int, int, unsigned int);
struct S2 { int w[2]; };
extern struct S2 data_ov081_02128ee4;
int func_ov081_021273e8(char *c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, (void*)data_ov081_02128ee4.w[1], 0x40000000, 0x1000, 0);
    *(int*)(c+0x130) = 0x1000;
    *(int*)(c+0x98) = 0;
    *(int*)(c+0x3e0) = 2;
    return 1;
}
