typedef unsigned char u8;
typedef unsigned int u32;

extern void func_ov030_021141a8(void *a, int m);
extern int Vec3_Dist(void *a, void *b);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *a, void *self, unsigned int, void *, unsigned int, unsigned int);
extern void func_0201267c(int, void *);
extern int _ZN6Player12GetTalkStateEv(void *p);
extern void _ZN6Player9DropActorEv(void *p);
extern u8 DecIfAbove0_Byte(u8 *p);
extern u8 data_0209d684;

int func_ov030_02112da0(char *a) {
    int b = (int)((*(u32 *)(a + 0xb0) & 0x40000) != 0);
    if (b != 0) {
        int p = (int)((long long)(int)(*(char **)(a + 0x3a8) + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
        *(int *)(a + 0x5c) = *(int *)p;
        *(int *)(a + 0x60) = *(int *)(p + 4);
        *(int *)(a + 0x64) = *(int *)(p + 8);
    }

    {
        u32 flags = *(u32 *)(a + 0xb0);
        b = (int)((flags & 0x80000) != 0);
        if (b != 0) {
            *(int *)(a + 0x3b8) = 1;
            func_ov030_021141a8(a, 7);
            return 1;
        }

        switch (*(u8 *)(a + 0x3c7)) {
        case 0: {
            int b2 = (int)((flags & 0x40000) != 0);
            if (b2 != 0) {
                char *s = *(char **)(a + 0x3a8);
                int off = 0x3c7;
                int *p = (int *)((long long)(int)(s + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
                int x = *p;
                u8 *st = (u8 *)(((int)a + off) & 0xFFFFFFFFFFFFFFFFLL);
                *(int *)(a + 0x5c) = x;
                *(int *)(a + 0x60) = p[1];
                *(int *)(a + 0x64) = p[2];
                (*st)++;
            } else {
                int b3 = (int)((flags & 0x20000) != 0);
                if (b3 != 0) break;
                if (b2 != 0) break;
                *(int *)(a + 0xd0) = 0;
                func_ov030_021141a8(a, *(int *)(a + 0x3b8));
            }
            break;
        }
        case 1:
            if (Vec3_Dist(a + 0x380, a + 0x5c) < 0x514000 &&
                *(int *)(a + 0x60) > *(int *)(a + 0x384) - 0x12c000) {
                if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(char **)(a + 0x3a8), a, 0xc1, 0, 0, 0) != 0) {
                    func_0201267c(0xd1, a + 0x74);
                    (*(u8 *)(((int)a + 0x3c7) & 0xFFFFFFFFFFFFFFFFLL))++;
                }
            }
            {
                char *s = *(char **)(a + 0x3a8);
                u8 val = 0x3c;
                int *p = (int *)((long long)(int)(s + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
                *(int *)(a + 0x5c) = *p;
                *(int *)(a + 0x60) = p[1];
                *(int *)(a + 0x64) = p[2];
                *(u8 *)(a + 0x3c6) = val;
            }
            break;
        case 2:
            if (_ZN6Player12GetTalkStateEv(*(char **)(a + 0x3a8)) == -1) {
                u8 g = data_0209d684;
                if (g == 1) {
                    _ZN6Player9DropActorEv(*(char **)(a + 0x3a8));
                    *(int *)(a + 0x3b8) = 8;
                    func_ov030_021141a8(a, 7);
                } else if (g == 2) {
                    (*(u8 *)(((int)a + 0x3c7) & 0xFFFFFFFFFFFFFFFFLL))++;
                }
            }
            break;
        case 3:
            if (DecIfAbove0_Byte((u8 *)(((int)a + 0x3c6) & 0xFFFFFFFFFFFFFFFFLL)) == 0) {
                *(u8 *)(a + 0x3c7) = 1;
            }
            break;
        }
    }
    return 1;
}
