typedef int s32;
extern void *data_ov006_02133a5c;
extern void Hud_RenderSprite(void *fn, s32 a, s32 b, s32 c, s32 d);

void func_ov006_0211e460(char *c)
{
    int i;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char *)(c + 0x48aa) != 0) {
            int v0 = *(int *)(c + 0x48a0) >> 12;
            int v4 = *(int *)(c + 0x48a4) >> 12;
            if (v4 <= 8) v4 = 8;
            if (v4 >= 0xb8) v4 = 0xb8;
            Hud_RenderSprite(data_ov006_02133a5c, v0, v4, -1, -1);
        }
        c += 0xc;
    }
}
