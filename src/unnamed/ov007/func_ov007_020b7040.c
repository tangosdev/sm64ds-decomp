extern void func_ov007_020bcf90(void);
extern void func_ov007_020bc9c8(int x);
struct G { int counter; char pad[0x70]; int f74; };
extern struct G *data_ov007_0210342c;
extern int data_ov007_02103430;
int func_ov007_020b7040(void) {
    func_ov007_020bcf90();
    func_ov007_020bc9c8(data_ov007_0210342c->f74);
    data_ov007_0210342c->counter++;
    data_ov007_02103430 = 1;
    return 1;
}
