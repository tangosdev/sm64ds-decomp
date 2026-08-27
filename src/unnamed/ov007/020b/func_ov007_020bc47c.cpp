//cpp
struct BMD_File;

extern "C" void _ZN5Model13LoadTexAndPalER8BMD_File(struct BMD_File *f);
extern "C" void func_020462d0(void *a, void *b, int c);
extern "C" void func_020462b4(int *p, int v);
extern "C" void func_ov007_020bc434(void *c);

extern "C" void func_ov007_020bc47c(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *m = *(unsigned char **)c;

    if ((*(int *)(c + 0x9c) & 8) == 0) {
        _ZN5Model13LoadTexAndPalER8BMD_File(*(struct BMD_File **)m);
    }
    func_020462d0(m, *(void **)m, *(int *)(c + 4));
    if ((*(int *)(c + 0x9c) & 4) != 0) {
        func_020462b4((int *)m, *(int *)(c + 0xc));
    }
    func_ov007_020bc434(c);

    {
        int cnt = *(int *)(*(unsigned char **)(*(unsigned char **)c) + 0x24);
        int i = 0;
        if (cnt > 0) {
            do {
                int *arr = *(int **)(c + 0x84);
                arr[i] = *(int *)(*(unsigned char **)(*(unsigned char **)c + 4) + i * 0x30 + 0x1c);
                i++;
            } while (i < cnt);
        }
    }

    *(int *)(c + 0x94) = 1;
    *(int *)(c + 0x98) = 1;
    *(unsigned short *)(c + 0x8e) = 0;
    *(unsigned short *)(c + 0x8c) = *(short *)(c + 0x8e);
}
