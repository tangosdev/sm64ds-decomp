extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern int data_ov085_021304e4[];
int func_ov085_0212a150(char *c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void*)data_ov085_021304e4[1], 0, 0x1000, 0);
    *(int*)(c + 0x98) = 0x4000;
    *(int*)(c + 0x354) = 3;
    return 1;
}
