typedef long long s64;
extern int GetGameLanguage(void);
extern void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);
extern void **data_ov006_021404c8;
extern void **data_ov006_0213b0d8[];
extern short data_02082214[];

void func_ov006_020c8af4(char *c)
{
    void *x;
    int t;
    int prod;
    int pos;
    if (*(short*)(c + 0x18) == 0) return;
    if (data_ov006_021404c8 != 0) {
        x = data_ov006_021404c8[*(short*)(c + 0x1a)];
    } else {
        int idx2 = GetGameLanguage();
        x = data_ov006_0213b0d8[idx2][*(short*)(c + 0x1a)];
    }
    t = data_02082214[(*(unsigned short*)(c + 0x1c) >> 4) * 2];
    prod = (int)(((s64)*(int*)(c + 0x10) * t + 0x800) >> 12);
    pos = *(int*)c + prod;
    Hud_RenderSprite(x, pos >> 12, *(int*)(c + 4) >> 12, -1, -1);
}
