extern int data_020a0db0;
extern unsigned short data_ov006_0213d600[];
extern int data_ov006_0214250c[];
void Hud_RenderSprite(void* a0, int a1, int a2, int a3, int a4);

void func_ov006_020f7e2c(char *thiz)
{
    unsigned char k;
    if (*(unsigned char*)(thiz + 0x2e) == 0) return;
    k = *(unsigned char*)(thiz + 0x2d);
    if (k == 0) return;
    if (k == 3) {
        if (((&data_020a0db0)[0] & 8) != 0) return;
    }
    {
        int idx = (*(unsigned char*)(thiz + 0x2c) + 1) * 5
                  + (*(int*)(thiz + 0x24) >> 12);
        unsigned short e = data_ov006_0213d600[idx];
        Hud_RenderSprite(
            (void*)data_ov006_0214250c[e],
            (*(int*)(thiz + 0xc) >> 12) + 0x18,
            (*(int*)(thiz + 0x10) >> 12) + 0x18,
            -1,
            -1);
    }
}
