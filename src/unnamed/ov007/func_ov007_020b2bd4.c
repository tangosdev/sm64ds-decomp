extern int func_ov007_020b3f20(int x);
extern void func_02044efc(void* a);
extern void* func_ov007_020bc6d4(void* a, int b);
extern void func_ov007_020bc6b4(void* c, void* r1, int r2);
extern void func_ov007_020bc5fc(void* c, void* r1, int r2);
extern void func_ov007_020bc658(void* c, void* r1, int r2);
extern void func_02045ef8(void* a, void* b);
extern void func_02046234(void* a, void* b);
extern void func_ov007_020bbf98(void* a, int b);
extern int data_ov007_020ccc48[];
extern char data_ov007_020ccc68[];
extern char data_ov007_020ccc80[];
extern int data_ov007_02103348[];
extern int data_ov007_02103354[];
extern char data_ov007_020d7ee8[];

void* func_ov007_020b2bd4(int a, int b)
{
    void* p;
    int j, k;
    int *b80, *b68;
    if (a == 0 && b == 0)
        k = 2;
    else
        k = a;
    p = (void*)func_ov007_020b3f20(data_ov007_020ccc48[a]);
    b80 = (int*)(data_ov007_020ccc80 + k * 0xc);
    b68 = (int*)(data_ov007_020ccc68 + a * 0xc);
    for (j = 0; j < 3; j++) {
        data_ov007_02103348[j] = func_ov007_020b3f20(b80[j]);
        data_ov007_02103354[j] = func_ov007_020b3f20(b68[j]);
    }
    func_02044efc(p);
    p = func_ov007_020bc6d4(p, 6);
    func_ov007_020bc6b4(p, data_ov007_02103348, 3);
    func_ov007_020bc5fc(p, data_ov007_02103354, 3);
    func_ov007_020bc658(p, data_ov007_020d7ee8 + k * 0xc, 3);
    func_02045ef8(*(void**)p, (void*)0x3def);
    func_02046234(*(void**)p, (void*)2);
    func_ov007_020bbf98(p, 3);
    return p;
}
