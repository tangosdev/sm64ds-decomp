extern void* data_ov077_02127c98[];
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* self, void* bca, int frame, int speed, unsigned int flags);
int func_ov077_02126a84(char *c) {
    *(int*)(c + 0x98) = 0;
    *(short*)(c + 0x100) = 0x46;
    *(int*)(c + 0x39c) = 0x1000;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x340, (void*)data_ov077_02127c98[1], 0, 0x1000, 0);
    return 1;
}
