//cpp
typedef struct Mtx { int m[12]; } Mtx;
extern "C" {
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *mtx, int rad, int h, unsigned int g);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov002_020e7d84(void *c);
extern void _ZN12CylinderClsn5ClearEv(void *p);
extern void _ZN12CylinderClsn6UpdateEv(void *p);

extern unsigned char data_0209f208;
extern unsigned char *data_0209f344;
extern Mtx data_02082128;

int _ZN10StarMarker8BehaviorEv(char *c) {
    if ((unsigned int)(*(unsigned char *)(c + 0x1db) << 0x1c) >> 0x1f) {
        if (*(unsigned char *)(c + 0x1d9) == data_0209f344[data_0209f208]) {
            *(unsigned short *)(c + 0x1d4) = 0;
            if (((unsigned int)(*(unsigned char *)(c + 0x1db) << 0x1f) >> 0x1f) == 0) {
                *(unsigned char *)((((int)c) + 0x1db) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
                *(int *)((((int)c) + 0xec) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
            }
        } else {
            *(unsigned short *)(c + 0x1d4) = 0x2a;
        }
        *(unsigned char *)((((int)c) + 0x1db) & 0xFFFFFFFFFFFFFFFFLL) &= ~8;
    }
    if (*(unsigned char *)(c + 0x1d8) != 0) {
        if (*(unsigned short *)(c + 0x1d4) != 0) {
            if (((unsigned int)(*(unsigned char *)(c + 0x1db) << 0x1e) >> 0x1f) == 0) {
                if (*(unsigned char *)(c + 0x1d9) == data_0209f344[data_0209f208]) {
                    *(unsigned short *)((((int)c) + 0x1d4) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
                    if (*(unsigned short *)(c + 0x1d4) == 0) {
                        if (((unsigned int)(*(unsigned char *)(c + 0x1db) << 0x1f) >> 0x1f) == 0) {
                            *(unsigned char *)((((int)c) + 0x1db) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
                            *(int *)((((int)c) + 0xec) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
                        }
                    }
                }
            }
        }
        Matrix4x3_FromTranslation(c + 0x130, *(int *)(c + 0x5c) >> 3, *(int *)(c + 0x60) >> 3,
                                  *(int *)(c + 0x64) >> 3);
    } else {
        *(short *)((((int)c) + 0x8e) & 0xFFFFFFFFFFFFFFFFLL) += 0x400;
        Matrix4x3_FromRotationY(c + 0x130, *(short *)(c + 0x8e));
        *(int *)(c + 0x154) = *(int *)(c + 0x5c) >> 3;
        *(int *)(c + 0x158) = *(int *)(c + 0x60) >> 3;
        *(int *)(c + 0x15c) = *(int *)(c + 0x64) >> 3;
    }
    if ((unsigned int)(*(unsigned char *)(c + 0x1db) << 0x1e) >> 0x1f) {
        *(Mtx *)(c + 0x18c) = data_02082128;
        *(int *)(c + 0x1b0) = *(int *)(c + 0x5c) >> 3;
        *(int *)(c + 0x1b4) = *(int *)(c + 0x60) >> 3;
        *(int *)(c + 0x1b8) = *(int *)(c + 0x64) >> 3;
        {
            int d = *(int *)(c + 0x60) - *(int *)(c + 0x1c8);
            int rad = 0xa0000;
            if (*(unsigned char *)(c + 0x1d8) != 0)
                rad = 0xc8000;
            _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                c, c + 0x164, c + 0x18c, rad, d + 0x28000, 0xf);
        }
    }
    if (*(unsigned char *)(c + 0x1d8) != 0) {
        if ((unsigned int)(*(unsigned char *)(c + 0x1db) << 0x1e) >> 0x1f) {
            if (*(unsigned char *)(c + 0x1d8) != 2 && *(int *)(c + 0xf8) != 0) {
                char *a = _ZN5Actor10FindWithIDEj(*(int *)(c + 0xf8));
                if (a != 0) {
                    if ((*(int *)(c + 0xf4) & 0x408000) != 0) {
                        *(int *)(c + 0x1d0) = (int)a;
                        func_ov002_020e7d84(c);
                        return 1;
                    }
                }
            }
            _ZN12CylinderClsn5ClearEv(c + 0xd4);
            _ZN12CylinderClsn6UpdateEv(c + 0xd4);
        }
    }
    return 1;
}
}
