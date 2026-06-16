extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern void* data_ov077_02127c90[];

int func_ov077_02126a04(char *c) {
    *(short*)(c + 0x420) = *(short*)(c + 0x8e) + 0x4000;
    *(int*)(c + 0x39c) = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x340, data_ov077_02127c90[1], 0, 0x1000, 0);
    return 1;
}
