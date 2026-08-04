extern void func_ov004_020b2444(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern void *data_ov006_02135054[];
extern void *data_ov006_02136de8[];

void func_ov006_02104d94(char *obj)
{
    if (*(unsigned char *)(obj + 0x4000 + 0x676) == 0) return;
    if (*(unsigned char *)(obj + 0x4000 + 0x675) != 4) {
        int a = *(int *)(obj + 0x4000 + 0x660) >> 12;
        int b = *(int *)(obj + 0x4000 + 0x664) >> 12;
        int idx;
        func_ov004_020b2444(a, b, *(unsigned char *)(obj + 0x4000 + 0xfde), -1, -1, 0, 0);
        idx = 1;
        if (*(unsigned char *)(obj + 0x4000 + 0xfde) >= 2) idx = 0;
        func_ov004_020afdd0(data_ov006_02135054[idx], a + 0x2c, b, -1, -1);
    } else {
        int x = *(unsigned char *)(obj + 0x4000 + 0x677);
        Hud_RenderSprite(data_ov006_02136de8[x],
                            *(int *)(obj + 0x4000 + 0x660) >> 12,
                            *(int *)(obj + 0x4000 + 0x664) >> 12,
                            -1, -1);
    }
}
