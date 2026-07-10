typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern void func_ov006_02104eb8(char *p);
extern void func_ov006_02104e70(unsigned char *p);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_02105854(char *c)
{
    if (*(u16 *)(c + 0x4ec4) != 0) {
        (*(u16 *)(int)(((long long)(int)(c + 0x4ec4)) & 0xFFFFFFFFFFFFFFFFLL))--;
        if (*(s16 *)(c + 0x4ec4) < 0)
            *(u16 *)(c + 0x4ec4) = 0;
        return;
    }

    {
        int found = 0;
        int i;
        int n = *(int *)(c + 0x4cb8);
        for (i = 0; i < n; i++) {
            if (*(u8 *)(c + i + 0x4efa) != 0) {
                found++;
                break;
            }
        }
        if (found)
            return;
    }

    if (*(u8 *)(c + 0x4fe4) == 0xff) {
        *(u8 *)(c + 0x4fe4) = 0;
        *(int *)(c + 0x4ca8) = 2;
        func_ov006_02104eb8(c);
        func_ov006_02104e70((unsigned char *)c);
        return;
    }

    (*(u8 *)(int)(((long long)(int)(c + 0x4fe4)) & 0xFFFFFFFFFFFFFFFFLL))--;

    if (*(u8 *)(c + 0x4fe4) == 0xff) {
        *(u8 *)(c + 0x4fe4) = 0;
        *(int *)(c + 0x4ca8) = 2;
        *(u8 *)(c + 0x4fe1) = 0;
        func_ov006_02104eb8(c);
        func_ov006_02104e70((unsigned char *)c);
        return;
    }

    {
        u8 slot = *(u8 *)(c + 0x4fe4);
        int hq = 3;
        int idx = *(u8 *)(c + slot + 0x4fce);
        int w = *(int *)(c + 0x4cbc);
        int wq = 3;
        int col = idx % w;
        int row = idx / w;
        int x0 = 0;
        int x, y;

        if (col == 0 || col == w - 1)
            wq = 2;
        if (row == 0 || row == w - 1)
            hq = 2;
        if (--col < 0)
            col = 0;
        if (--row < 0)
            row = 0;

        for (y = 0; y < hq; y++) {
            for (x = x0; x < wq; x++) {
                int cell = *(int *)(c + 0x4cbc) * (row + y) + (col + x);
                *(u8 *)(c + cell + 0x4efa) = 1;
                (*(u8 *)(int)(((long long)(int)(c + cell + 0x4f1e)) & 0xFFFFFFFFFFFFFFFFLL)) ^= 1;
                *(u16 *)(c + cell * 2 + 0x4e78) = 8;
                *(u8 *)(c + cell + 0x4f8a) = 1;
            }
        }
        *(u16 *)(c + idx * 2 + 0x4e78) = 0;
        _ZN5Sound12PlayBank2_2DEj(0x1fb);
    }
}
