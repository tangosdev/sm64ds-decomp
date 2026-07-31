extern int *data_ov006_0213bf34[];
extern int *func_020beb68;
int func_ov004_020ad674(void);
void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);
void func_ov004_020b1de8(int r0, int r1, int r2, int r3);
void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3);
void func_ov004_020b2444(int a0, int a1, int a2, int a3, int a4, int a5, int a6);

void func_ov006_020dcd74(char *thiz)
{
    int idx;
    int r2;
    if (*(int*)(thiz + 0x5000 + 0x1c8) < 2)
        return;
    idx = func_ov004_020ad674();
    RenderOamMainScreen(data_ov006_0213bf34[idx][0x10/4], 0x80, 0x50, -1, -1);
    func_ov004_020b1de8(0x6c, 0x60, 1, -1);
    idx = func_ov004_020ad674();
    DrawOamSprite((void*)data_ov006_0213bf34[idx][4/4], (void*)0x7e, 0x60, (void*)0);
    if (func_020beb68 != 0)
        r2 = *(int*)((char*)func_020beb68 + 0xac);
    else
        r2 = 0;
    func_ov004_020b2444(0x90, 0x60, r2, 1, -1, 2, 0);
}
