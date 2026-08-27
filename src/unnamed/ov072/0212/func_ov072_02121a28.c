extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern int data_ov072_02122c94[];
int func_ov072_02121a28(char *c) {
    *(int*)(c + 0x98) = 0x19000;
    *(int*)(c + 0xa8) = 0xc000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void*)data_ov072_02122c94[1], 0x40000000, 0x1000, 0);
    *(unsigned char*)(c + 0x36e) = 0;
    *(int*)(c + 0x368) = 1;
    return 1;
}
