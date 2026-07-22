typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern void func_ov006_02104cfc(char *c, int i);

void func_ov006_021051dc(char *c)
{
    int idx;
    int w;
    int col;
    int row;
    int hq;
    int wq;
    int y;
    int x;

    if (*(u16 *)(c + 0x4ec4) != 0) {
        (*(u16 *)(int)(((long long)(int)(c + 0x4ec4)) &
                       0xFFFFFFFFFFFFFFFFLL))--;
        if (*(s16 *)(c + 0x4ec4) < 0)
            *(u16 *)(c + 0x4ec4) = 0;
        return;
    }

    if (*(u8 *)(c + 0x4fe5) >= *(u8 *)(c + 0x4fde)) {
        *(u16 *)(c + 0x4ec0) = 0x50;
        *(int *)(c + 0x4ca8) = 6;
        *(u8 *)(c + 0x4fe6) = 0;
        return;
    }

    idx = *(u8 *)(c + *(u8 *)(c + 0x4fe5) + 0x4fae);
    w = *(int *)(c + 0x4cbc);
    hq = 3;
    wq = hq;

    col = idx % w;
    row = idx / w;

    if (col == 0 || col == w - 1)
        wq = 2;
    if (row == 0 || row == w - 1)
        hq = 2;

    if (--col < 0)
        col = 0;
    if (--row < 0)
        row = 0;

    for (y = 0; y < hq; y++) {
        for (x = 0; x < wq; x++) {
            int cell = *(int *)(c + 0x4cbc) * (row + y) + (col + x);

            *(u8 *)(c + cell + 0x4efa) = 1;
            (*(u8 *)(int)(((long long)(int)(c + cell + 0x4f1e)) &
                          0xFFFFFFFFFFFFFFFFLL)) ^= 1;
            *(u16 *)(c + cell * 2 + 0x4e78) = 8;
        }
    }

    func_ov006_02104cfc(c, idx);
    *(u16 *)(c + idx * 2 + 0x4e78) = 0;
    *(u16 *)(c + 0x4ec4) = 0x30;
    (*(u8 *)(int)(((long long)(int)(c + 0x4fe5)) &
                  0xFFFFFFFFFFFFFFFFLL))++;
}
