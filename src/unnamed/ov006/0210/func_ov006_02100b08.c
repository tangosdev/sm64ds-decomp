extern void func_ov006_02102c3c(void *self, int x, int z, int flag);

void func_ov006_02100b08(char *self, int idx)
{
    char *base = self + idx * 64;

    if (*(unsigned short *)(base + 0x5292) != 0) {
        *(unsigned short *)(self + 0x5292 + idx * 64) -= 1;
        if (*(short *)(base + 0x5292) < 0) {
            *(unsigned short *)(base + 0x5292) = 0;
        }
        return;
    }
    if (*(unsigned char *)(base + 0x5299) != 0) {
        int x;
        int z;

        *(unsigned char *)(self + 0x5299 + idx * 64) -= 1;
        *(unsigned short *)(base + 0x5292) = 0x30;
        x = *(int *)(base + 0x5260) >> 12;
        z = *(int *)(base + 0x5264) >> 12;
        func_ov006_02102c3c(self, x, z, *(unsigned char *)(base + 0x5299) & 1);
    }
}
