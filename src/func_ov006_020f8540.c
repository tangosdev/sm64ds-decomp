extern int data_ov006_0213d56c;
extern int data_ov006_02133f18;
extern void func_ov004_020b1ea4(int a, int b, int c, int d, int e, int f, int g);
extern void Hud_RenderSprite(int a, int b, int c, int d, int e);

void func_ov006_020f8540(void)
{
    int g = data_ov006_0213d56c;
    if (g == 0) return;
    func_ov004_020b1ea4(0xe8, 0x28, g, -1, -1, 0, 0);
    Hud_RenderSprite(data_ov006_02133f18, 0xe8, 0x18, -1, -1);
}
