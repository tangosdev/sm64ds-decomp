extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);

void func_ov006_0211dec0(void *arg) {
    int i;
    char *p = (char *)arg;
    for (i = 0; i < 16; i++, p += 0x14) {
        unsigned char v;
        if (*(unsigned char *)(p + 0x4a70) == 0) {
            continue;
        }
        v = *(unsigned char *)(p + 0x4a73);
        if (v == 0) {
            if (*(int *)(p + 0x4a6c) != 0) {
                *(int *)(((long long)(int)(p + 0x4a6c)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
                if ((short)*(int *)(p + 0x4a6c) < 0) {
                    *(int *)(p + 0x4a6c) = 0;
                }
            } else {
                _ZN5Sound12PlayBank2_2DEj(0x1bc);
                *(int *)(p + 0x4a68) = -0x400;
                *(int *)(p + 0x4a6c) = 0x10;
                *(unsigned char *)(((long long)(int)(p + 0x4a73)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
                *(unsigned char *)(p + 0x4a71) = 1;
            }
        } else if (v == 1) {
            *(int *)(((long long)(int)(p + 0x4a64)) & 0xFFFFFFFFFFFFFFFFLL) += *(int *)(p + 0x4a68);
            *(int *)(((long long)(int)(p + 0x4a68)) & 0xFFFFFFFFFFFFFFFFLL) -= 0x80;
            if (*(int *)(p + 0x4a6c) != 0) {
                *(int *)(((long long)(int)(p + 0x4a6c)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
                if ((short)*(int *)(p + 0x4a6c) < 0) {
                    *(int *)(p + 0x4a6c) = 0;
                }
            } else {
                *(int *)(p + 0x4a6c) = 0x18;
                *(unsigned char *)(((long long)(int)(p + 0x4a73)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
            }
        }
    }
}
