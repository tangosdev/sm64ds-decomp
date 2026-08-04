typedef int s32;
extern unsigned short data_ov006_0213bd64[];
extern void *data_ov006_02141774[];
extern void Hud_RenderSprite(void *fn, s32 a, s32 b, s32 c, s32 d);

void func_ov006_020d9bdc(char *c)
{
    int idx;
    unsigned short e;
    if (*(unsigned char *)(c + 0x2b) == 0) return;
    idx = (*(unsigned char *)(c + 0x2a) + 1) * 5 + (*(int *)(c + 0x24) >> 12);
    e = data_ov006_0213bd64[idx];
    Hud_RenderSprite(
        data_ov006_02141774[e],
        (*(int *)(c + 4) >> 12) + 0x18,
        (*(int *)(c + 8) >> 12) + 0x18,
        -1, -1);
}
