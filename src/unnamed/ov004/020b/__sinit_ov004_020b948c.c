struct B8 { int a, b; };
struct B16 { struct B8 p, q; };

extern void func_ov004_020ae104(void *c);
extern void func_020733a8(void *obj, int a, int b, void *cb1, void *cb2);
extern void func_020731dc(int a, int b, void **node);
extern char data_ov004_020beb74[];
extern char data_ov004_020bebe8[];
extern void func_ov004_020b2c7c(void);
extern void func_ov004_020b2c80(void);
extern void func_ov004_020b2c58(void);
extern void *data_ov004_020beb7c[];
extern struct B8 data_ov004_020bbf4c;
extern struct B8 data_ov004_020bbf54;
extern struct B8 data_ov004_020bbf5c;
extern struct B8 data_ov004_020bbf64;
extern struct B16 data_ov004_020beb88;
extern struct B16 data_ov004_020beb98;

void __sinit_ov004_020b948c(void)
{
    func_ov004_020ae104(data_ov004_020beb74);
    func_020733a8(data_ov004_020bebe8, 0x40, 0x20, (void *)func_ov004_020b2c80, (void *)func_ov004_020b2c7c);
    func_020731dc(0, (int)func_ov004_020b2c58, data_ov004_020beb7c);
    data_ov004_020beb88.p = data_ov004_020bbf4c;
    data_ov004_020beb88.q = data_ov004_020bbf5c;
    data_ov004_020beb98.p = data_ov004_020bbf54;
    data_ov004_020beb98.q = data_ov004_020bbf64;
}
