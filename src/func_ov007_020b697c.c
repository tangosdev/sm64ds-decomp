extern int func_ov007_020b3f20(int n);
extern int func_ov007_020b2308(int a, int b);
struct SomeObj {
    int pad[0x78/4];
    int items[0x10];
};
extern struct SomeObj *data_ov007_0210342c;
void func_ov007_020b697c(void)
{
    int i;
    int n = func_ov007_020b3f20(0x16);
    for (i = 0; i < 0xb; i++) {
        int r = func_ov007_020b2308(n, i);
        data_ov007_0210342c->items[i] = r;
    }
}
