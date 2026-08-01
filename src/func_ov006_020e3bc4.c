extern void func_ov004_020af948(int a, int b, int c, int d);
extern void DrawOamSprite(int a, int b, int c, int d);
extern int data_ov006_0213a5e0[];

void func_ov006_020e3bc4(char *c)
{
    int i;
    for (i = 0; i < 0x32; i++) {
        if (*(unsigned char *)(c + 0x4000 + 0x8e0)) {
            int x = *(int *)(c + 0x4000 + 0x8c0) >> 0xc;
            int y = *(int *)(c + 0x4000 + 0x8c4) >> 0xc;
            func_ov004_020af948(data_ov006_0213a5e0[*(unsigned char *)(c + 0x4000 + 0x8e1)], x, y, 0);
            DrawOamSprite(data_ov006_0213a5e0[*(unsigned char *)(c + 0x4000 + 0x8e2)], x, y, 0);
        }
        c += 0x24;
    }
}
