#pragma scheduling off
extern int fn_02383BF4(void);
extern int fn_02384B2C(int a1);
extern int fn_0238647C(int a1);
extern int fn_023869CC(int a1);
extern void fn_02383438(void);
extern void fn_0238332C(int a1, int a2, int a3, int a4);
extern int fn_02383364(int a1);
extern int fn_02383B9C(int a1);
extern int fn_02383B70(int a1);
extern int fn_02384960(void);
extern void fn_02386D08(int a1);
extern void fn_023863F0(int a1);
extern int fn_02384664(int a1);
extern void fn_02386758(int a1);
extern unsigned fn_02383828(void);

void fn_02383A88(void) {
    int cur;
    int one;
    int zero;
    int flag;
    int status;
    int z;
    fn_023869CC(fn_0238647C(fn_02384B2C(fn_02383BF4())));
    fn_02383438();
    z = 0;
    fn_0238332C(z, z, z, z);
    cur = fn_02383B9C(fn_02383364(0x7F));
    one = 1;
    zero = 0;
    for (;;) {
        flag = zero;
        status = fn_02383B70(cur);
        switch (status) {
        case 1:
            flag = one;
            break;
        case 2:
            break;
        default:
            break;
        }
        fn_02386D08(fn_02384960());
        fn_023863F0(flag);
        fn_02386758(fn_02384664(flag));
        cur = (int)fn_02383828();
    }
}
