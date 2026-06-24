extern void func_ov007_020ca5f0(int a, int b, void* v, int d, int e);
extern void func_ov007_020cb3dc(int x);
struct V3 { int x, y, z; };
extern struct V3 data_ov007_020d7d60;
extern int *data_ov007_02104bd4;
void func_ov007_020bee14(void)
{
    struct V3 v = data_ov007_020d7d60;
    v.z += 0x200;
    func_ov007_020ca5f0(data_ov007_02104bd4[0], data_ov007_02104bd4[1], &v, 0x3e8, 1);
    func_ov007_020cb3dc(data_ov007_02104bd4[0]);
    v.z += 0x200;
    func_ov007_020ca5f0(data_ov007_02104bd4[0], data_ov007_02104bd4[2], &v, 0x3e8, 1);
}
