//cpp
extern "C" {
extern void func_ov007_020c43bc(void*, int);
extern char* data_ov007_0210342c;
extern unsigned char data_ov007_020d7dc8[];
extern unsigned char data_ov007_020d7dd0[];
extern unsigned char data_ov007_020d7dd1[];
extern unsigned char data_ov007_020d7ddc[];
extern unsigned char data_ov007_020d7ddd[];
extern unsigned char data_ov007_020d7dc0[];
extern short data_ov007_020d7de8[];
extern short data_ov007_020d7dea[];

/* The fields func_ov007_020c43bc reads back: it scales pixelWidth and
 * pixelHeight by |depth| and the camera's field of view (over 96, half the
 * screen height) into world-space sizes at +0x20/+0x24, so they are a
 * size in pixels and the z distance it is drawn at. The other fields set
 * here (+0x5c, +0x70, +0x72, +0x7d, +0x7e, +0x80) are table loads whose
 * roles this file does not show, so they stay raw. */
struct SpriteView {
    char pad_00[0xc];
    short pixelWidth;  /* 0x0c */
    short pixelHeight; /* 0x0e */
    int depth;         /* 0x10 */
};
void func_ov007_020b3d30(char* thiz, int idx){
    SpriteView *sprite = (SpriteView *)thiz;
    *(*(int **)(thiz + 0x5c)) =
        *(int *)((char *)data_ov007_0210342c + 0x78 + data_ov007_020d7dc8[idx] * 4);
    *(short *)(thiz + 0x70) = data_ov007_020d7dd0[idx * 2];
    *(short *)(thiz + 0x72) = data_ov007_020d7dd1[idx * 2];
    sprite->pixelWidth = data_ov007_020d7ddc[idx * 2];
    sprite->pixelHeight = data_ov007_020d7ddd[idx * 2];
    *(char *)(thiz + 0x7d) = data_ov007_020d7dc0[idx];
    *(short *)(thiz + 0x7e) = *(short *)((char *)data_ov007_020d7de8 + idx * 4);
    *(short *)(thiz + 0x80) = *(short *)((char *)data_ov007_020d7dea + idx * 4);
    if (idx == 1)
        sprite->depth = -0x10c0;
    else
        sprite->depth = (int)0xffffef38;
    func_ov007_020c43bc(thiz, *(int *)((char *)data_ov007_0210342c + 0x34));
}
}
