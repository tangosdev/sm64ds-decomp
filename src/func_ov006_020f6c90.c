typedef unsigned char u8;
typedef unsigned int u32;

extern int RandomIntInternal(int *seed);
extern void func_ov006_020f6f88(char *obj);
extern int data_0209d4b8;

void func_ov006_020f6c90(char *c)
{
    for (int i = 0; i < 11; i++)
        *(u8 *)(c + i + 0x53f2) = 0;
    for (int i = 0; i < 8; i++)
        *(u8 *)(c + i + 0x53fd) = 0xff;

    u8 flag = *(u8 *)(c + 0x540a);
    if (flag == 0) {
        int i = 0;
        char *p = c;
        for (; i < 16; i++, p += 0x18) {
            u32 idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15);
            u8 *entry;
        check0:
            entry = (u8 *)(((long long)(int)(c + idx + 0x53f2)) & 0xFFFFFFFFFFFFFFFFLL);
            if (*entry < 2) {
                *(u8 *)(p + 0x51b8) = idx;
                (*entry)++;
            } else {
                idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15);
                goto check0;
            }
        }
        int j = 0;
        char *q = c;
        for (; j < 16; j++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        func_ov006_020f6f88(c);
        return;
    } else if (flag == 1) {
        int i = 0;
        char *p = c;
        for (; i < 18; i++, p += 0x18) {
            u32 idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15);
            u8 *entry;
        check1:
            entry = (u8 *)(((long long)(int)(c + idx + 0x53f2)) & 0xFFFFFFFFFFFFFFFFLL);
            if (*entry < 2) {
                *(u8 *)(p + 0x51b8) = idx;
                (*entry)++;
            } else {
                idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15);
                goto check1;
            }
        }
        int j = 0;
        char *q = c;
        for (; j < 18; j++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        func_ov006_020f6f88(c);
        return;
    } else {
        int i = 0;
        char *p = c;
        for (; i < 20; i++, p += 0x18) {
            u32 idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 10 >> 15);
            u8 *entry;
        check2:
            entry = (u8 *)(((long long)(int)(c + idx + 0x53f2)) & 0xFFFFFFFFFFFFFFFFLL);
            if (*entry < 2) {
                *(u8 *)(p + 0x51b8) = idx;
                (*entry)++;
            } else {
                idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 10 >> 15);
                goto check2;
            }
        }
        int j = 0;
        char *q = c;
        for (; j < 20; j++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        func_ov006_020f6f88(c);
        return;
    }
}
