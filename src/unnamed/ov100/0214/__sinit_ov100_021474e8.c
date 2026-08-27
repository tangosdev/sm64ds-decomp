struct P2 { int a, b; };
extern struct P2 data_ov100_02148668;
extern struct P2 data_ov100_02148670;
extern struct P2 data_ov100_02147f38;
extern struct P2 data_ov100_02147f18;
extern struct P2 data_ov100_02147f20;
extern struct P2 data_ov100_02147f30;
extern struct P2 data_ov100_02147f28;
extern struct P2 data_ov100_0214867c;
extern int func_02017ab4(int x);
int func_02017acc(void *x, int y);
void func_020731dc(void *a, void *b, void **node);

void __sinit_ov100_021474e8(void)
{
    func_02017acc(&data_ov100_02148668, 0x9c00);
    func_020731dc(&data_ov100_02148668, (void*)&func_02017ab4,
                  (void**)&data_ov100_02148670);
    (&data_ov100_0214867c)[0] = data_ov100_02147f38;
    (&data_ov100_0214867c)[1] = data_ov100_02147f18;
    (&data_ov100_0214867c)[2] = data_ov100_02147f20;
    (&data_ov100_0214867c)[3] = data_ov100_02147f30;
    (&data_ov100_0214867c)[4] = data_ov100_02147f28;
}
