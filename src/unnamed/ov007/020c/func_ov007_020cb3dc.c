typedef short s16;

extern void func_ov007_020cb7c0(int a, int b, int c, int d, int e, int f);
extern void func_ov007_020cb4b0(char* p, int a, int b, int c, int x, int y);
extern void func_ov007_020c368c(void* p, int n, int k);

void func_ov007_020cb3dc(char* self)
{
    int x, y;
    int* t = *(int**)(self + 4);
    int w1 = t[0] - 1;
    int h1 = t[1] - 1;
    char* p;

    func_ov007_020cb7c0(*(int*)(self + 0x28), w1, h1, *(int*)self,
                        *(s16*)(self + 0x24), *(s16*)(self + 0x26));

    p = *(char**)(self + 0x2c);
    for (y = 0; y < (*(int**)(self + 4))[1]; y++) {
        for (x = 0; x < (*(int**)(self + 4))[0]; x++) {
            func_ov007_020cb4b0(p, *(int*)(self + 0x28), w1, h1, x, y);
            p += 6;
        }
    }

    func_ov007_020c368c(*(void**)(self + 0x2c),
                        (*(int**)(self + 4))[0] * (*(int**)(self + 4))[1], 1);
}
