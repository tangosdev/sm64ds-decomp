extern void func_ov007_020c1d8c(int i);
extern void func_ov007_020b20e8(void);
extern void func_ov007_020b4b5c(int a, int b);
struct S {
    int f0;        /* 0 */
    char pad[0x10-4];
    int f10;       /* 0x10 */
    int f14;       /* 0x14 */
    int f18;       /* 0x18 */
    int f1c;       /* 0x1c */
    int f20;       /* 0x20 */
    int f24;       /* 0x24 */
    char pad2[0x114-0x28];
    int arr[];     /* 0x114 */
};
extern struct S *data_ov007_0210342c;
void func_ov007_020b6544(void)
{
    func_ov007_020c1d8c(0);
    data_ov007_0210342c->f18 = 0;
    data_ov007_0210342c->f1c = 1;
    data_ov007_0210342c->f10 = 0;
    data_ov007_0210342c->f14 = 0;
    data_ov007_0210342c->f0 = 0;
    *(int*)((char*)data_ov007_0210342c + 0x180) = -1;
    data_ov007_0210342c->f24 = 0;
    data_ov007_0210342c->f20 = 1;
    func_ov007_020b20e8();
    int i;
    for (i = 0; i < 0x18; i++) {
        func_ov007_020b4b5c(data_ov007_0210342c->arr[i], *(int*)((char*)data_ov007_0210342c + 0x34));
    }
}
