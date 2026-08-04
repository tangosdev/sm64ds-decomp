void func_ov006_020fc9b0(char *base, int i)
{
    int x, y, cnt;

    if (*(unsigned char *)(base + 0x4000 + i * 0x38 + 0x68f) == 5) return;
    x = *(int *)(base + 0x4000 + i * 0x38 + 0x660) >> 12;
    y = *(int *)(base + 0x4000 + i * 0x38 + 0x664) >> 12;
    cnt = 0;
    if (y >= 0xd0) cnt++;
    if (x >= 0x120 || x <= -0x20) cnt++;
    if (cnt != 0) {
        *(unsigned char *)(base + 0x4000 + i * 0x38 + 0x68c) = 0;
        *(unsigned char *)(base + 0x4000 + i * 0x38 + 0x68d) = 0;
    }
}
