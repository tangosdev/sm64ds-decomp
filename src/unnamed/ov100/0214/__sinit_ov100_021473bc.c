extern int func_02017acc();
extern void func_020731dc();
extern int _ZN13SharedFilePtr9ConstructEj();
extern char data_ov100_02148608[];
extern char data_ov100_02148600[];
extern char data_ov100_02148610[];
extern char data_ov100_0214861c[];
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);

struct Pair { int a, b; };
extern struct Pair data_ov100_02147e48;
extern struct Pair data_ov100_02147e40;
extern struct Pair data_ov100_02147e50;
extern struct Pair data_ov100_02147e28;
extern struct Pair data_ov100_02147e30;
extern struct Pair data_ov100_02147e20;
extern struct Pair data_ov100_02147e58;
extern struct Pair data_ov100_02147e38;
extern struct Pair data_ov100_02148628[8];

void __sinit_ov100_021473bc(void)
{
    func_02017acc(data_ov100_02148608, 0x43a);
    func_020731dc(data_ov100_02148608, func_02017ab4, data_ov100_02148610);
    _ZN13SharedFilePtr9ConstructEj(data_ov100_02148600, 0x43b);
    func_020731dc(data_ov100_02148600, SharedFilePtr_Destruct_Anim, data_ov100_0214861c);
    data_ov100_02148628[0] = data_ov100_02147e48;
    data_ov100_02148628[1] = data_ov100_02147e40;
    data_ov100_02148628[2] = data_ov100_02147e50;
    data_ov100_02148628[3] = data_ov100_02147e28;
    data_ov100_02148628[4] = data_ov100_02147e30;
    data_ov100_02148628[5] = data_ov100_02147e20;
    data_ov100_02148628[6] = data_ov100_02147e58;
    data_ov100_02148628[7] = data_ov100_02147e38;
}
