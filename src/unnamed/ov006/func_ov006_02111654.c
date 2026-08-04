extern void func_ov004_020afdd0(int a, int b, int c, int d, int e);
extern int data_ov006_02138318[];
void func_ov006_02111654(char *c)
{
    if (*(unsigned char*)(c + 0x31) == 1) {
        int v = *(int*)(c + 0x34);
        int idx;
        if (v < 0x14) idx = 2;
        else if (v < 0x28) idx = 1;
        else idx = 0;
        if (idx >= 1) {
            func_ov004_020afdd0(data_ov006_02138318[idx], *(int*)(c + 8) >> 0xc, *(int*)(c + 0xc) >> 0xc, -1, 2);
        }
    }
    func_ov004_020afdd0(data_ov006_02138318[0], *(int*)(c + 8) >> 0xc, *(int*)(c + 0xc) >> 0xc, -1, 3);
}
