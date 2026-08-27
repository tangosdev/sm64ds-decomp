typedef int s32;

extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[][4];
extern unsigned char data_020a0de9[][4];
extern unsigned char data_020a0dea[][4];
extern unsigned char data_020a0deb[][4];
extern void func_ov006_020fbd38(char *self);

void func_ov006_020fe2e4(char *self, int i)
{
    unsigned int idx = data_020a0e40;
    int flag = 0;
    char *p;
    int dx, dy;

    if (data_020a0de8[idx][0] != 0) {
        flag = data_020a0de9[idx][0] != 0;
    }
    if (flag != 0) {
        p = self + i * 0x38;
        dx = ((*(s32 *)(p + 0x4ed8)) >> 12) - data_020a0dea[idx][0];
        dy = ((*(s32 *)(p + 0x4edc)) >> 12) - data_020a0deb[idx][0];
        *(unsigned char *)(p + 0x4f0d) = 1;
        *(s32 *)(p + 0x4ee8) = dx << 12;
        *(s32 *)(p + 0x4eec) = dy << 12;
        *(unsigned char *)(p + 0x4f0f) = 0;
    }
    func_ov006_020fbd38(self);
}
