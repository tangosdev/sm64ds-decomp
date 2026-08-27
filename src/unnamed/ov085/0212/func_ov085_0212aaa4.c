extern void func_02013944(void);
extern int data_ov085_021305c0[];
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, void *, int, int, unsigned int);

int func_ov085_0212aaa4(void *c) {
    *(int *)((char *)c + 0x41c) = 0;
    func_02013944();
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char *)c + 0x300, (void *)data_ov085_021305c0[1], 0, 0x1000, 0);
    return 1;
}
