extern void func_02017acc();
extern void func_020731dc();
extern int func_02017ab4[];

extern int data_ov096_02137b20[];
extern int data_ov096_02137b30[];
extern int data_ov096_02137b28[];
extern int data_ov096_02137b3c[];

struct P { int a, b; };
extern struct P data_ov096_02137948;
extern struct P data_ov096_02137940;
extern struct P data_ov096_02137950;
extern struct P data_ov096_02137928;
extern struct P data_ov096_02137930;
extern struct P data_ov096_02137920;
extern struct P data_ov096_02137978;
extern struct P data_ov096_02137970;
extern struct P data_ov096_02137938;
extern struct P data_ov096_02137968;
extern struct P data_ov096_02137960;
extern struct P data_ov096_02137958;
extern struct P data_ov096_02137b48[12];

void __sinit_ov096_0213770c(void)
{
    func_02017acc(data_ov096_02137b20, 0x41b);
    func_020731dc(data_ov096_02137b20, func_02017ab4, data_ov096_02137b30);
    func_02017acc(data_ov096_02137b28, 0x41a);
    func_020731dc(data_ov096_02137b28, func_02017ab4, data_ov096_02137b3c);

    data_ov096_02137b48[0] = data_ov096_02137948;
    data_ov096_02137b48[1] = data_ov096_02137940;
    data_ov096_02137b48[2] = data_ov096_02137950;
    data_ov096_02137b48[3] = data_ov096_02137928;
    data_ov096_02137b48[4] = data_ov096_02137930;
    data_ov096_02137b48[5] = data_ov096_02137920;
    data_ov096_02137b48[6] = data_ov096_02137978;
    data_ov096_02137b48[7] = data_ov096_02137970;
    data_ov096_02137b48[8] = data_ov096_02137938;
    data_ov096_02137b48[9] = data_ov096_02137968;
    data_ov096_02137b48[10] = data_ov096_02137960;
    data_ov096_02137b48[11] = data_ov096_02137958;
}
