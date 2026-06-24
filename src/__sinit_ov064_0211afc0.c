extern int func_02017acc();
extern void func_020731dc();
extern int func_02017b4c();
extern char data_ov064_0211c730[];
extern char data_ov064_0211c738[];
extern char data_ov064_0211c728[];
extern char data_ov064_0211c744[];
extern void func_02017ab4(void);
extern void func_02017b34(void);

struct P2 { int a, b; };
extern struct P2 data_ov064_0211bc0c;
extern struct P2 data_ov064_0211bc14;
extern struct P2 data_ov064_0211bc1c;
struct P6 { struct P2 x, y, z; };
extern struct P6 data_ov064_0211c750;

void __sinit_ov064_0211afc0(void) {
    func_02017acc(data_ov064_0211c730, 0x5ef);
    func_020731dc(data_ov064_0211c730, func_02017ab4, data_ov064_0211c738);
    func_02017b4c(data_ov064_0211c728, 0x5f0);
    func_020731dc(data_ov064_0211c728, func_02017b34, data_ov064_0211c744);
    data_ov064_0211c750.x = data_ov064_0211bc0c;
    data_ov064_0211c750.y = data_ov064_0211bc14;
    data_ov064_0211c750.z = data_ov064_0211bc1c;
}
