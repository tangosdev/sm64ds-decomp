extern int func_ov007_020c3df4(int a, void* b);
extern void* func_ov007_020c3f00(int* r6, void* r4);
extern int func_ov007_020c3ee8(int a, int b);
extern void func_ov007_020c108c(char *c, int r1, int r2, int r3, int sp0);
extern void func_0204dc84(char *p);
extern void func_ov007_020be8b8(void);

extern int* data_ov007_0210342c;
extern int* data_ov007_02104bc0;

extern int data_ov007_020d7c6c;
extern int data_ov007_02103234;

void func_ov007_020be50c(void)
{
    int* g;
    int r4v;
    int* t;
    int* n;
    int* e;
    int* ef0;
    int* t2;
    int count;
    int i;
    int off;

    g = data_ov007_0210342c;
    r4v = g[0x74/4];
    t = *(int**)(r4v + 0x3c);

    data_ov007_02104bc0 = (int*)func_ov007_020c3df4(0, (void*)0x108);
    data_ov007_02104bc0[0] = (int)func_ov007_020c3f00(&data_ov007_020d7c6c, (void*)2);

    data_ov007_02104bc0[1] = func_ov007_020c3df4((int)&data_ov007_02103234, (void*)(data_ov007_020d7c6c * 0xc));

    func_ov007_020c3ee8(data_ov007_02104bc0[0], *(int*)(data_ov007_0210342c[0x74/4] + 4));

    n = *(int**)data_ov007_02104bc0;
    count = **(int**)n;
    i = 0;
    if (count > 0)
    {
        off = i;
        do
        {
            e = ((int**)n[2])[i];
            t2 = (int*)t[2];
            ef0 = (int*)e[0];
            *(int*)(ef0[6]) = t2[3] + 0x10;
            *(char*)((char*)e + 0xa) = 8;
            func_ov007_020c108c((char*)e[0], 0, 1, 0, 0);
            func_0204dc84((char*)((char*)data_ov007_02104bc0[1] + off));
            n = *(int**)data_ov007_02104bc0;
            i++;
            off += 0xc;
            count = **(int**)n;
        } while (i < count);
    }

    e = (int*)n[3];
    *(int*)(((int*)e[0])[6]) = ((int*)t[2])[3] + 0x20;
    ef0 = (int*)e[0];
    *(int*)(ef0[6] + 4) = ((int*)t[2])[3];
    *(char*)((char*)e + 0xa) = 0x20;

    func_ov007_020be8b8();
}
