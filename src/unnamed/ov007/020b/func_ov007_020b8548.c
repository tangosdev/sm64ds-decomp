extern void func_ov007_020c951c(int unused, void *h, void *extra);
extern void MultiStore32Bytes(unsigned int val, int *dst, int len);
extern void func_ov007_020c0308(unsigned short *dst, int b, int c);
extern void MultiCopy32Bytes(int *src, int *dst, int len);
extern void func_ov007_020b8860(int r0, int r1);
extern void func_ov007_020b797c(void);

extern void *data_ov007_02102ef8;
extern int data_ov007_020d7a5c[3];
extern short data_ov007_02103458;
extern int data_ov007_0210345c;
extern int data_ov007_02103464;
extern int data_ov007_02103468;
extern int data_ov007_02102dd8;

typedef struct { int a, b, c; } Trio;
extern Trio data_ov007_02103478;

extern unsigned short data_ov007_02103998[];
extern unsigned short data_ov007_02104598[];
extern unsigned short data_ov007_02103f98[];

void func_ov007_020b8548(void *unused0, int p1, int p2, void *p3, void *p4, int p5, void *p6)
{
    func_ov007_020c951c(0, p6, &data_ov007_02102ef8);

    data_ov007_02103458 = (short)p2;
    data_ov007_0210345c = p1;
    data_ov007_02103464 = 0;
    data_ov007_02103468 = 0;

    data_ov007_02102dd8 = 0xe;

    data_ov007_02103478.a = (int)p3;
    data_ov007_02103478.b = (int)p4;
    data_ov007_02103478.c = p5;

    {
        volatile int v1 = 0;
        MultiStore32Bytes(v1, (int*)data_ov007_02103478.a, data_ov007_020d7a5c[0]);
    }
    {
        volatile int v2 = 0;
        MultiStore32Bytes(v2, (int*)data_ov007_02103478.b, data_ov007_020d7a5c[1]);
    }

    if (p5 != 0) {
        volatile int v3 = 0;
        MultiStore32Bytes(v3, (int*)data_ov007_02103478.c, data_ov007_020d7a5c[2]);
    }

    func_ov007_020c0308(data_ov007_02103998, 0x18, 0xf);
    MultiCopy32Bytes((int*)data_ov007_02103998, (int*)data_ov007_02104598, 0x600);
    MultiCopy32Bytes((int*)data_ov007_02103998, (int*)data_ov007_02103f98, 0x600);

    func_ov007_020b8860(0, 4);
    func_ov007_020b797c();
}
