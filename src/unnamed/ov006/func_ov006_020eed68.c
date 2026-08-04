extern int func_ov004_020b2444(int, int, int, int, int, int, int);
extern int data_ov006_021421b4[];

void func_ov006_020eed68(char *c)
{
    if (*(short *)(c + 0x20) == 0) return;
    func_ov004_020b2444(*(int *)(c + 4) >> 12, *(int *)(c + 8) >> 12,
                        *(short *)(c + 0x1e), -1, -1, 0, data_ov006_021421b4[0]);
}
