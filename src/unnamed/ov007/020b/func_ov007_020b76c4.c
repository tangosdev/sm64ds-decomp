extern int func_02049d78(void* a, void* b);
extern void* data_ov007_02103450[];
extern void* data_ov007_0210344c;
void func_ov007_020b76c4(int c)
{
    void* r1 = data_ov007_02103450[c];
    if (!r1) return;
    func_02049d78(data_ov007_0210344c, r1);
    data_ov007_02103450[c] = 0;
}
