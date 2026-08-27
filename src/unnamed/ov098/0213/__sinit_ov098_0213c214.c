extern void func_02017acc();
extern void func_020731dc();
extern void func_02017ab4(void);
extern int data_ov098_0213c8e8[];
extern int data_ov098_0213c8f0[];
struct W2 { int a, b; };
extern struct W2 data_ov098_0213c64c;
extern struct W2 data_ov098_0213c65c;
extern struct W2 data_ov098_0213c654;
extern struct W2 data_ov098_0213c644;
extern struct W2 data_ov098_0213c8fc[];
void __sinit_ov098_0213c214(void)
{
    func_02017acc(data_ov098_0213c8e8, 0x45e);
    func_020731dc(data_ov098_0213c8e8, &func_02017ab4, data_ov098_0213c8f0);
    data_ov098_0213c8fc[0] = data_ov098_0213c64c;
    data_ov098_0213c8fc[1] = data_ov098_0213c65c;
    data_ov098_0213c8fc[2] = data_ov098_0213c654;
    data_ov098_0213c8fc[3] = data_ov098_0213c644;
}
