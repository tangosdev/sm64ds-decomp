extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*, void*, int, int, unsigned int);
extern int _ZN9Animation7AdvanceEv(void*);
extern void* data_ov085_021304d4[];

int func_ov085_0212a328(char *c) {
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0xd4, data_ov085_021304d4[1], 0, 0x1000, 0);
    *(int*)(c + 0x12c) = 0;
    _ZN9Animation7AdvanceEv(c + 0x124);
    *(char*)(c + 0x368) = 1;
    *(int*)(c + 0x354) = 1;
    return 1;
}
