extern int _ZN8dActor_c23HorzAngleToCPlayerOrAngEv(void *);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, int, int, int, unsigned int);
extern int data_ov070_02123518[];
int func_ov070_0211fa80(char *c) {
    *(int *)(c + 0x3dc) = 0;
    *(short *)(c + 0x100) = 0x3f;
    *(int *)(c + 0x3d8) = 0;
    *(short *)(c + 0x3e6) = _ZN8dActor_c23HorzAngleToCPlayerOrAngEv(c);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov070_02123518[1], 0x40000000, 0x1000, 0);
    return 1;
}
