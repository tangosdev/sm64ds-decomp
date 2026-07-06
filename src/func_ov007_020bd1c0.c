typedef unsigned short u16;

typedef struct { int a, b, c; } S12;

extern char* data_ov007_0210342c;
extern S12 data_ov007_020d7c34[];
extern S12 data_ov007_020d7c28;

extern void func_ov007_020bce70(short n, int m);
extern void* func_ov007_020b3f20(int idx);
extern void func_ov007_020b22e0(int* a, int* b, int c);
extern void func_ov007_020b1fa4(int self);
extern void func_ov007_020bd4a0(int r0);
extern void func_ov007_020b2998(void);
extern void func_ov007_020bfbdc(void* self);
extern void func_ov007_020c14b8(void* t);
extern void func_ov007_020b77ac(void);
extern void func_ov007_020b9880(int a, int b);
extern void func_ov007_020bf428(void);
extern void func_ov007_020b3edc(int r0);

void func_ov007_020bd1c0(int n)
{
    int res = 0;
    int i;
    void* h;

    func_ov007_020bce70(0, 0);

    *(S12*)(*(char**)(data_ov007_0210342c + 0x38) + 0x10) = data_ov007_020d7c34[n];

    h = func_ov007_020b3f20(0x16);
    for (i = 0; i < 11; i++) {
        func_ov007_020b22e0(((int**)(data_ov007_0210342c + 0x78))[i], (int*)h, i);
    }

    {
        int t = *(unsigned char*)(*(char**)(data_ov007_0210342c + 0x28) + 0x3a) - 2;
        if (t >= 0) {
            void* h2;
            res = t + 0x20;
            h2 = func_ov007_020b3f20(res);
            func_ov007_020b22e0(*(int**)(data_ov007_0210342c + 0x80), (int*)h2, 0);
            func_ov007_020b22e0(*(int**)(data_ov007_0210342c + 0x90), (int*)h2, 1);
        }
    }

    switch (n) {
    case 0:
        {
            volatile u16* reg = (volatile u16*)0x4000060;
            func_ov007_020b1fa4(1);
            *reg = *reg & ~0x3002;
            *(S12*)(*(char**)(data_ov007_0210342c + 0x3c) + 0x10) = data_ov007_020d7c28;
            *(short*)(*(char**)(data_ov007_0210342c + 0x3c) + 0x22) = 0x18c6;
            func_ov007_020bd4a0(0);
            func_ov007_020b2998();
            *(int*)(data_ov007_0210342c + 0xf8) = 0;
            func_ov007_020bfbdc(*(void**)(data_ov007_0210342c + 0x30));
            func_ov007_020bd4a0(1);
            func_ov007_020c14b8(*(void**)(data_ov007_0210342c + 0x98));
            func_ov007_020c14b8(*(void**)(data_ov007_0210342c + 0x94));
            func_ov007_020c14b8(*(void**)(data_ov007_0210342c + 0x90));
            func_ov007_020c14b8(*(void**)(data_ov007_0210342c + 0x9c));
            func_ov007_020c14b8(*(void**)(data_ov007_0210342c + 0xa0));
            func_ov007_020b77ac();
        }
        break;
    case 1:
        func_ov007_020b1fa4(2);
        func_ov007_020bd4a0(0);
        func_ov007_020bfbdc(*(void**)(data_ov007_0210342c + 0x30));
        func_ov007_020bd4a0(1);
        func_ov007_020c14b8(*(void**)(data_ov007_0210342c + 0x7c));
        func_ov007_020c14b8(*(void**)(data_ov007_0210342c + 0x84));
        func_ov007_020c14b8(*(void**)(data_ov007_0210342c + 0x88));
        func_ov007_020b9880(*(int*)(data_ov007_0210342c + 0x20), *(int*)(data_ov007_0210342c + 0xec));
        break;
    case 2:
        {
            volatile u16* reg = (volatile u16*)0x4000060;
            func_ov007_020b1fa4(3);
            *reg = (*reg & ~0x3002) | 2;
            func_ov007_020bce70(4, 0);
            func_ov007_020bd4a0(0);
            func_ov007_020bfbdc(*(void**)(data_ov007_0210342c + 0x30));
            func_ov007_020bd4a0(1);
            func_ov007_020c14b8(*(void**)(data_ov007_0210342c + 0x80));
            func_ov007_020c14b8(*(void**)(data_ov007_0210342c + 0x78));
            func_ov007_020c14b8(*(void**)(data_ov007_0210342c + 0xa0));
            func_ov007_020bf428();
        }
        break;
    }

    func_ov007_020b3edc(0x16);
    if (res != 0) {
        func_ov007_020b3edc(res);
    }
}
