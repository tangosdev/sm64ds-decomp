extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* anim, void* file, int a, int b, unsigned int u);
extern int data_ov081_02128d98[];
int func_ov081_02124ec0(char *p) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(p+0x30c, (void*)data_ov081_02128d98[1], 0, 0x1000, 0);
    if (*(int*)(p+0x41c) == 2)
        *(int*)(p+0xb0) = 0x18000002;
    else
        *(int*)(p+0xb0) = 0x10000002;
    *(short*)(p+0x100) = 0x14;
    return 1;
}
