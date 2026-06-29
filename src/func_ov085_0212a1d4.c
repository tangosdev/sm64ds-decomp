extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern int data_ov085_021304c4[];
int func_ov085_0212a1d4(char *c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void*)data_ov085_021304c4[1], 0, 0x1000, 0);
    *(int*)(c + 0x98) = 0x1800;
    *(int*)(c + 0x354) = 2;
    return 1;
}
