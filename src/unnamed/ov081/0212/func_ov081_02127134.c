extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char*, void*, int, int, unsigned int);
extern int data_ov081_02128edc[];

int func_ov081_02127134(char *c) {
    *(int*)(c + 0x98) = 0xa000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void*)data_ov081_02128edc[1], 0, 0x1000, 0);
    *(int*)(c + 0x130) = 0x1000;
    *(int*)(c + 0x3e0) = 5;
    return 1;
}
