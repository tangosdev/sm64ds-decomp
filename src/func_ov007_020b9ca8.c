/* func_ov007_020b9ca8 @ 0x020b9ca8 (ov007, size 0x264)
 * Minigame cursor/pen update: advances the shared pen state, spawns the
 * touch feedback (sound id 0x25..0x28 by rhythm), fades the music with pen
 * hold time, and updates the frame counter at +0x28.
 */
typedef unsigned short u16;
typedef unsigned int u32;

extern char *data_ov007_0210342c;
extern char *data_ov007_02104ba0;

extern void func_ov007_020bb4b0(void);
extern int func_ov007_020c1da0(int);
extern void func_ov007_020bfe4c(char *, int, int, int, int *);
extern void func_ov007_020c5a6c(char *, int *, int);
extern int _ZN4cstd3modEii(int, int);
extern void func_ov007_020bdeb0(int);
extern void func_ov007_020bbc08(int, int, int);
extern int _ZN4cstd3divEii(int, int);
extern void func_ov007_020c6314(char *, int, int);
extern void func_ov007_020c5fc8(char *, int, int);
extern void func_ov007_020c5948(char *, int *);

void func_ov007_020b9ca8(void)
{
    char *r5 = *(char **)data_ov007_02104ba0;
    char *r4 = *(char **)(data_ov007_0210342c + 0x50);
    int v[3];

    func_ov007_020bb4b0();

    if (*(u16 *)(r4 + 0xc) != 0 && *(u32 *)(r4 + 0x20) >= 1 &&
        func_ov007_020c1da0(0) == 0) {
        char *x;
        *(int *)(data_ov007_02104ba0 + 0x1c) = 0;
        x = *(char **)(data_ov007_0210342c + 0x30);
        func_ov007_020bfe4c(x, *(u16 *)(r4 + 8) << 12, *(u16 *)(r4 + 0xa) << 12,
                            -*(int *)(x + 0x2c), v);
        if (*(u32 *)(r4 + 0x20) == 1) {
            int s;
            func_ov007_020c5a6c(r5, v, 0);
            s = 0x25;
            if (_ZN4cstd3modEii(*(int *)(data_ov007_02104ba0 + 0x24), 3) == 2) {
                switch (_ZN4cstd3modEii(*(int *)(data_ov007_02104ba0 + 0x24), 9)) {
                case 2:
                    s = 0x26;
                    break;
                case 5:
                    s = 0x27;
                    break;
                case 8:
                    s = 0x28;
                    break;
                }
            }
            func_ov007_020bdeb0(s);
        }
    } else if (*(int *)(r4 + 0x24) == 0) {
        u32 t = *(u32 *)(r4 + 0x20);
        if (t <= 5) {
            func_ov007_020bbc08(0, 0, 0);
        } else if (*(int *)(*(char **)data_ov007_02104ba0 + 0x58) != -1) {
            int f;
            char *snd;
            if ((int)t <= 5)
                f = 0;
            else if ((int)t >= 0x12c)
                f = 0x1000;
            else
                f = _ZN4cstd3divEii((t - 5) << 12, 0x127);
            snd = *(char **)data_ov007_02104ba0;
            if (*(int *)(snd + 8) >= 2)
                func_ov007_020c6314(snd, *(int *)(snd + 0x58), 2);
            func_ov007_020c5fc8(*(char **)data_ov007_02104ba0,
                                *(int *)(*(char **)data_ov007_02104ba0 + 0x58), f);
        }
        *(int *)(r5 + 0x5c) = -1;
        *(int *)(r5 + 0x58) = *(int *)(r5 + 0x5c);
        *(int *)(r5 + 0x68) = 0;
        *(int *)(r5 + 0x64) = *(int *)(r5 + 0x68);
        *(int *)(r5 + 0x60) = *(int *)(r5 + 0x64);
    }

    func_ov007_020c5948(r5, v);

    if (*(u16 *)(r4 + 0xc) != 0 ||
        *(u16 *)(*(char **)(data_ov007_0210342c + 0x54)) != 0) {
        *(int *)(data_ov007_02104ba0 + 0x28) = 0;
    } else {
        (*(int *)(int)(((long long)(int)(data_ov007_02104ba0 + 0x28)) & 0xFFFFFFFFFFFFFFFFLL))++;
    }
}
