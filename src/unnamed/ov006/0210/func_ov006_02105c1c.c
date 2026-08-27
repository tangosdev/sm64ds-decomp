void func_ov006_02105c1c(char *base)
{
    int i = 0;

    if (*(int *)(base + 0x4cb8) <= 0) return;
    do {
        if (*(unsigned char *)(base + i + 0x4f1e) != *(unsigned char *)(base + i + 0x4f42)) {
            *(unsigned char *)(base + i + 0x4efa) = 2;
            *(unsigned short *)(base + (int)(((long long)i)) * 2 + 0x4e30) = 0;
        }
        i++;
    } while (i < *(int *)(base + 0x4cb8));
}
