extern void func_02017acc();
extern void func_020731dc();
extern void func_02017b4c();
extern void func_02017ab4(void);
extern void func_02017b34(void);

extern int data_ov010_02112d08[];
extern int data_ov010_02112d10[];
extern int data_ov010_02112d00[];
extern int data_ov010_02112d1c[];

typedef struct { int a, b; } S8;
extern S8 data_ov010_02112a78;
extern S8 data_ov010_02112a88;
extern S8 data_ov010_02112a80;
extern S8 data_ov010_02112a98;
extern S8 data_ov010_02112a90;
typedef struct { S8 v0, v1, v2, v3, v4; } S40;
extern S40 data_ov010_02112d28;

void __sinit_ov010_0211203c(void)
{
    func_02017acc(data_ov010_02112d08, 0x5af);
    func_020731dc(data_ov010_02112d08, &func_02017ab4, data_ov010_02112d10);
    func_02017b4c(data_ov010_02112d00, 0x5b0);
    func_020731dc(data_ov010_02112d00, &func_02017b34, data_ov010_02112d1c);
    data_ov010_02112d28.v0 = data_ov010_02112a78;
    data_ov010_02112d28.v1 = data_ov010_02112a88;
    data_ov010_02112d28.v2 = data_ov010_02112a80;
    data_ov010_02112d28.v3 = data_ov010_02112a98;
    data_ov010_02112d28.v4 = data_ov010_02112a90;
}
