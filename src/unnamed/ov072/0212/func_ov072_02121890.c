extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);
extern int data_ov072_02122c9c[];
int func_ov072_02121890(char *c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, (void*)data_ov072_02122c9c[1], 0, 0x1000, 0);
    *(int*)(c + 0x130) = 0x1000;
    *(int*)(c + 0x368) = 2;
    return 1;
}
