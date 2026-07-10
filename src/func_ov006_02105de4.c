/* func_ov006_02105de4 @ 0x02105de4 (ov006, size 0x264)
 * Touch-grid minigame: find the cursor slot whose fixed-point position is
 * within 16px of the level's target cell, mark it captured, flip a 2x2/3x3
 * quad of grid cells around it and start their flash timers.
 */
typedef unsigned char u8;
typedef unsigned short u16;

typedef struct Obj {
    char _p0[0x4cb8];
    int count;                       /* 0x4cb8 */
    int w;                           /* 0x4cbc */
    char _p1[4];
    int xarr[(0x4d54 - 0x4cc4) / 4]; /* 0x4cc4 */
    int yarr[(0x4de8 - 0x4d54) / 4]; /* 0x4d54 */
} Obj;

extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];

extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern void func_ov006_02104e80(char *);


void func_ov006_02105de4(char *c)
{
    u8 lvl;
    int n;
    int i;
    int tx, ty;

    if (*(u8 *)(c + 0x4fe9) != 0)
        return;

    lvl = data_020a0e40;
    n = 0;
    if (data_020a0de8[lvl * 4] != 0)
        n = data_020a0de9[lvl * 4] != 0;
    if (n == 0)
        return;

    i = 0;
    n = ((Obj *)c)->count;
    if (n <= 0)
        return;

    {
        u8 *e = &data_020a0de8[lvl * 4];
        tx = e[2];
        ty = e[3];
    }

    do {
        int dx = tx - (((Obj *)c)->xarr[i] >> 12);
        int dy = ty - (((Obj *)c)->yarr[i] >> 12);
        if (dx >= -0x10 && dx <= 0x10 && dy >= -0x10 && dy <= 0x10) {
            int wq = 3;
            int hq = 3;
            int w, col, row;

            *(u8 *)(c + *(u8 *)(c + 0x4fe4) + 0x4fce) = i;
            (*(u8 *)(int)(((long long)(int)(c + 0x4fe4)) & 0xFFFFFFFFFFFFFFFFLL))++;
            w = *(int *)(c + 0x4cbc);
            col = i % w;
            row = i / w;

            if (col == 0 || col == w - 1)
                wq = 2;
            if (row == 0 || row == w - 1)
                hq = 2;

            if (--col < 0)
                col = 0;
            if (--row < 0)
                row = 0;

            {
                int x0 = 0;
                int y;
                for (y = 0; y < hq; y++) {
                    int x;
                    for (x = x0; x < wq; x++) {
                        int idx = *(int *)(c + 0x4cbc) * (row + y) + (col + x);
                        *(u8 *)(c + idx + 0x4efa) = 1;
                        (*(u8 *)(int)(((long long)(int)(c + idx + 0x4f1e)) & 0xFFFFFFFFFFFFFFFFLL)) ^= 1;
                        *(u16 *)(c + idx * 2 + 0x4e78) = 8;
                    }
                }
            }

            *(u16 *)(c + i * 2 + 0x4e78) = 0;
            *(u8 *)(c + 0x4feb) = 0x28;
            (*(u8 *)(int)(((long long)(int)(c + 0x4fe1)) & 0xFFFFFFFFFFFFFFFFLL))++;
            _ZN5Sound12PlayBank2_2DEj(0x1fa);
            func_ov006_02104e80(c);
            return;
        }
    } while (++i < n);
}
