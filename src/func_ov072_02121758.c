extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern int data_ov072_02122cbc[];
int func_ov072_02121758(char *c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void*)data_ov072_02122cbc[1], 0, 0x1000, 0);
    *(int*)(c + 0x130) = 0x1000;
    *(int*)(c + 0x98) = 0x5000;
    *(int*)(c + 0x368) = 3;
    return 1;
}
