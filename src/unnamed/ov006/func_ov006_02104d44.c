void Hud_RenderSprite(void *a, int b, int c, int d, int e);
extern void *data_ov006_02136d40[];
void func_ov006_02104d44(char *c) {
    if (*(unsigned char*)(c + 0x4fe3) == 0) return;
    Hud_RenderSprite(data_ov006_02136d40[*(unsigned char*)(c + 0x4fe2)],
                        0xf0, 0x24, -1, -1);
}
