typedef unsigned char u8;
typedef unsigned short u16;

extern int data_020a0f5c;
extern u16 data_020a0f24;
extern u8 data_020a0f9d[4];
extern u16 data_020a0fbe[4][3];
extern u16 data_020a0fa0[][3];

extern int func_0203fbfc(void);
extern int func_0203faa8(void);
extern void func_0205a61c(void *dst, void *src, int n);
extern int func_02042748(u16 *a, u16 *b);
extern int func_02061d30(int self, unsigned int idx);

void func_02040014(char *a)
{
    int v;

    if (*(u16 *)(a + 2) != 0) {
        goto not_ready;
    }

    v = *(u16 *)(a + 8);
    switch (v) {
    case 0:
    {
        int sel = data_020a0f5c;
        data_020a0f24 = 0;
        switch (sel) {
        case 0:
            func_0203fbfc();
            break;
        case 3:
            if (func_0203faa8() != 0)
                return;
            data_020a0f5c = 0;
            func_0203fbfc();
            break;
        case 4:
            data_020a0f5c = 4;
            func_0203fbfc();
            break;
        }
        break;
    }
    case 2:
        break;
    case 7:
    {
        u16 buf[3];
        int matched;
        int cnt;
        u8 *flag_p;
        u16 *item_p;

        matched = 0;
        func_0205a61c(a + 0xa, buf, 6);

        flag_p = data_020a0f9d;
        item_p = data_020a0fbe[0];
        cnt = 1;
        do {
            if (*flag_p != 0) {
                if (func_02042748(buf, item_p) != 0) {
                    u16 idx = *(u16 *)(a + 0x10);
                    func_0205a61c(buf, data_020a0fa0[idx], 6);
                    matched = 1;
                    break;
                }
            }
            cnt++;
            flag_p++;
            item_p += 3;
        } while (cnt < 4);

        if (matched != 0) {
            return;
        }
        func_02061d30(0, *(u16 *)(a + 0x10));
        break;
    }
    case 9:
        break;
    }
    return;

not_ready:
    data_020a0f5c = 0;
    func_0203fbfc();
}
