struct S { void* f0; int* f4; void* f8; int* fc; };
extern void* func_ov007_020c3df4(int a, int b);
extern void MultiStore_Int(int val, int* dst, int len);

struct S* func_ov007_020c12ac(int a, int b)
{
    struct S* s = (struct S*)func_ov007_020c3df4(0, 0x10);
    s->f4 = (int*)func_ov007_020c3df4(0, a << 3);
    s->fc = (int*)func_ov007_020c3df4(0, b << 4);
    s->f0 = (void*)a;
    s->f8 = (void*)b;
    {
        volatile int v1;
        int i;
        for (i = 0; i < a; i++) {
            v1 = 0;
            MultiStore_Int(v1, (int*)((char*)s->f4 + i * 8), 8);
        }
    }
    {
        volatile int v2;
        int j;
        for (j = 0; j < b; j++) {
            v2 = 0;
            MultiStore_Int(v2, (int*)((char*)s->fc + j * 16), 0x10);
        }
    }
    return s;
}
