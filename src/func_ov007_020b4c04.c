typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern void *func_ov007_020c3df4(int a, int b);
extern void *func_ov007_020aea6c(int a, void *b, void *c);
extern void *func_ov007_020c44e4(u8 x);
extern void *func_ov007_020c908c(void *p);

extern s32 data_ov007_020d77d8[];
extern u8 data_ov007_020d76e8[];
extern u8 data_ov007_020d78f8[];
extern u8 data_ov007_020d77e0[];
extern u16 data_ov007_020d76a0[];
extern u16 data_ov007_020d76a1[];
extern u8 data_ov007_020d77e1[];
extern s32 data_ov007_020d7648[];
extern char *data_ov007_0210342c;
extern u16 data_ov007_020d75e8[];
extern u16 data_ov007_020d75f8[];
extern u8 data_ov007_020d76c0[];
extern u8 data_ov007_020d7680[];
extern u16 data_ov007_020d7608[];
extern u16 data_ov007_020d7600[];
extern u8 data_ov007_020d7658[];
extern u8 data_ov007_020d766c[];
extern u16 data_ov007_020d7628[];
extern u16 data_ov007_020d7638[];

void *func_ov007_020b4c04(int arg0)
{
    char *r7;
    char *r6;
    s32 r5;
    u8 t;

    r7 = (char *)func_ov007_020c3df4(0, 0x1c);
    r5 = *(s32 *)((char *)data_ov007_020d77d8 + arg0 * 0xc);
    *(void **)(r7) = func_ov007_020aea6c(arg0, (char *)data_ov007_020d76e8 + arg0 * 0xa,
                                         (char *)data_ov007_020d78f8 + r5 * 0x14);
    *(void **)(r7 + 4) = func_ov007_020c44e4(((u8 *)data_ov007_020d77e0)[*(u16 *)(*(char **)r7) * 0xc]);
    r6 = *(char **)(r7 + 4);
    if (arg0 == 0x15) {
        *(u8 *)(r6 + 0x4e) = 0x1c;
    } else {
        *(u8 *)(r6 + 0x4e) = 0x1b;
    }
    *(s32 *)(r6 + 0x64) = 3;
    *(s32 *)(r6 + 0x68) = 0;
    *(s32 *)(r6 + 0x6c) = 1;
    *(s16 *)(r6 + 0xc) = (s16)((u8 *)data_ov007_020d76a0)[*(s32 *)((char *)data_ov007_020d77d8 + *(u16 *)(*(char **)r7) * 0xc) * 2];
    *(s16 *)(r6 + 0xe) = (s16)((u8 *)data_ov007_020d76a1)[*(s32 *)((char *)data_ov007_020d77d8 + *(u16 *)(*(char **)r7) * 0xc) * 2];
    *(s32 *)(r6 + 0x10) = data_ov007_020d7648[((u8 *)data_ov007_020d77e1)[*(u16 *)(*(char **)r7) * 0xc]];
    *(s32 *)(r7 + 8) = 0;
    *(void **)(r7 + 0x10) = func_ov007_020c908c(r7 + 0xc);
    *(s32 *)(r7 + 0xc) = 0;
    *(s16 *)(r7 + 0x18) = *(s16 *)(*(char **)(*(char **)r7 + 0x24) + 2);
    *(s16 *)(r7 + 0x14) = *(s16 *)(r7 + 0x18);
    *(s16 *)(r7 + 0x1a) = *(s16 *)(*(char **)(*(char **)r7 + 0x24) + 4);
    *(s16 *)(r7 + 0x16) = *(s16 *)(r7 + 0x1a);

    if (arg0 >= 0 && arg0 <= 2) {
        
        *(void **)(*(char **)(r6 + 0x5c)) = *(void **)(data_ov007_0210342c + 0x90);
        *(u16 *)(r6 + 0x70) = data_ov007_020d75e8[arg0];
        *(u16 *)(r6 + 0x72) = data_ov007_020d75f8[arg0];
        if (*(u8 *)(*(char **)(data_ov007_0210342c + 0x28) + 0x3a) == 5) {
            *(void **)(*(char **)r7 + 0x28) = &data_ov007_020d76c0[(r5 - 1) * 0x14];
            *(void **)(*(char **)r7 + 0x24) = &data_ov007_020d7680[arg0 * 0xa];
            if (arg0 != 0) {
                *(s16 *)(r6 + 0xc) = 0x48;
            } else {
                s32 *p = (s32 *)(((long long)(int)(r6 + 0x10)) & 0xFFFFFFFFFFFFFFFFLL);
                *p = *p + 0xc;
            }
        }
    } else if (arg0 >= 3 && arg0 <= 6) {
        
        *(void **)(*(char **)(r6 + 0x5c)) = *(void **)(data_ov007_0210342c + 0x7c);
        *(u16 *)(r6 + 0x70) = data_ov007_020d7608[arg0 - 3];
        *(u16 *)(r6 + 0x72) = data_ov007_020d7600[arg0 - 3];
    } else if (arg0 >= 7 && arg0 <= 0xd) {
        if (arg0 == 7) {
            *(void **)(*(char **)(r6 + 0x5c)) = *(void **)(data_ov007_0210342c + 0x90);
        } else {
            *(void **)(*(char **)(r6 + 0x5c)) = *(void **)(data_ov007_0210342c + 0x98);
        }
        switch (arg0) {
        case 7:
            *(u16 *)(r6 + 0x70) = 0x60;
            break;
        case 8:
            *(u16 *)(r6 + 0x70) = 0xc0;
            *(u16 *)(r6 + 0x72) = 0x20;
            t = *(u8 *)(*(char **)(data_ov007_0210342c + 0x28) + 0x3a);
            switch (t) {
            case 3:
                *(void **)(*(char **)(r6 + 0x5c)) = *(void **)(data_ov007_0210342c + 0x94);
                *(u16 *)(r6 + 0x70) = 0;
                *(u16 *)(r6 + 0x72) = 0;
                *(s16 *)(r6 + 0xc) = 0x50;
                *(void **)(*(char **)r7 + 0x28) = &data_ov007_020d7658;
                break;
            case 4:
                *(void **)(*(char **)(r6 + 0x5c)) = *(void **)(data_ov007_0210342c + 0x94);
                *(u16 *)(r6 + 0x70) = 0x50;
                *(u16 *)(r6 + 0x72) = 0;
                *(s16 *)(r6 + 0xc) = 0x60;
                *(void **)(*(char **)r7 + 0x28) = &data_ov007_020d766c;
                break;
            }
            break;
        case 9:
            break;
        case 10:
            *(u16 *)(r6 + 0x70) = 0x90;
            *(u16 *)(r6 + 0x72) = 0x20;
            break;
        case 11:
            *(u16 *)(r6 + 0x70) = 0x90;
            *(u16 *)(r6 + 0x72) = 0x30;
            break;
        default:
            *(u16 *)(r6 + 0x72) = 0x20;
            break;
        }
    } else if (arg0 >= 0xe && arg0 <= 0x15) {
        
        *(void **)(*(char **)(r6 + 0x5c)) = *(void **)(data_ov007_0210342c + 0x80);
        *(u16 *)(r6 + 0x70) = data_ov007_020d7628[arg0 - 0xe];
        *(u16 *)(r6 + 0x72) = data_ov007_020d7638[arg0 - 0xe];
    } else if (arg0 >= 0x16 && arg0 <= 0x17) {
        
        *(void **)(*(char **)(r6 + 0x5c)) = *(void **)(data_ov007_0210342c + 0x80);
        *(u16 *)(r6 + 0x70) = (u16)((arg0 - 0x16) * 0x48);
        *(u16 *)(r6 + 0x72) = 0x80;
    }
    return r7;
}
