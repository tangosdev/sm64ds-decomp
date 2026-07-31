extern int *data_ov006_0213bf34[];
int func_ov004_020ad674(void);
void func_ov004_020afa20(int a0, int a1, int a2, int a3, int a4);
void func_ov004_020b1de8(int r0, int r1, int r2, int r3);
void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3);
void func_ov004_020b2444(int a0, int a1, int a2, int a3, int a4, int a5, int a6);

void func_ov006_020dccb8(char *thiz)
{
    int idx;
    if (*(int*)(thiz + 0x5000 + 0x1c8) < 2)
        return;
    idx = func_ov004_020ad674();
    func_ov004_020afa20(data_ov006_0213bf34[idx][0x20/4], 0x80, 0x18, -1, -1);
    func_ov004_020b1de8(0x68, 0x28, 1, -1);
    idx = func_ov004_020ad674();
    DrawOamSprite((void*)data_ov006_0213bf34[idx][4/4], (void*)0x7a, 0x28, (void*)0);
    func_ov004_020b2444(0x8c, 0x28, *(int*)(thiz + 0x5000 + 0x1d4), 1, -1, 2, 0);
}
