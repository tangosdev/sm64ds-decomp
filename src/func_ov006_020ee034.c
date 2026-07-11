typedef unsigned short u16;

typedef struct { int w[12]; } M48;
typedef struct { int w[3]; } V3;

extern void func_ov004_020b1e34(void *a, int b, int c, int d);
extern int func_ov004_020ad674(void);
extern void func_ov004_020afcf8(void *a0, void *a1, int a2, void *a3);
extern void func_ov004_020afa20(int a0, int a1, int a2, int a3, int a4);
extern void func_0203cd80(int *m, short angle);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void func_02045e44(void *self, unsigned int value, int index);
extern void func_ov006_020c70d0(void);
extern void func_ov006_020c425c(void);

extern int data_ov006_02140428;
extern int *data_ov006_0213cbb4[];
extern int data_ov006_02134cf8;
extern int data_020a0e68;
extern int data_ov006_0213cb8c[3];

int func_ov006_020ee034(void *self)
{
    char *c = (char *)self;

    if (*(u16 *)(c + 0x4664) == 1) {
        int count;
        int a1v;
        int i;

        func_ov004_020b1e34(self, 0xe0, 0x14, 1);

        count = data_ov006_02140428;
        a1v = 0x6e;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = func_ov004_020ad674();
                func_ov004_020afcf8((void *)data_ov006_0213cbb4[idx][1], (void *)a1v, 0xc, (void *)0);
            } else {
                func_ov004_020afa20(data_ov006_02134cf8, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    {
        int m[3];
        m[0] = 0;
        m[1] = 0;
        m[2] = 0xfffff008;
        func_0203cd80(m, -0x2000);
        *(volatile int *)0x40004c8 =
            (((short)m[0] >> 3) & 0x3ff) |
            ((((short)m[1] >> 3) & 0x3ff) << 10) |
            ((((short)m[2] >> 3) & 0x3ff) << 20);
        *(volatile int *)0x40004cc = 0x7fff;
        *(volatile int *)0x40004cc = 0x40007fff;
        Matrix4x3_FromTranslation(&data_020a0e68, 0, -0xef000, 0);
    }
    {
        int m[3];
        m[0] = 0;
        m[1] = 0;
        m[2] = 0xfffff008;
        func_0203cd80(m, -0x2000);
        *(volatile int *)0x40004c8 =
            ((((short)m[0] >> 3) & 0x3ff) |
            ((((short)m[1] >> 3) & 0x3ff) << 10) |
            ((((short)m[2] >> 3) & 0x3ff) << 20)) | 0x40000000;
    }

    *(M48 *)(c + 0x5038) = *(M48 *)&data_020a0e68;

    {
        V3 t;
        void *obj;
        void *p;

        t = *(V3 *)data_ov006_0213cb8c;

        obj = c + 0x5024;
        p = *(void **)((char *)obj + 4);
        func_02045e44(obj, 1, 0);
        *(int *)((char *)p + 0x18) = *(int *)(c + 0x500c);

        {
            void *obj2 = (void *)(((long long)(int)(c + 0x501c)) & 0xFFFFFFFFFFFFFFFFLL);
            ((void (**)(void *, void *))(*(int *)obj2))[5](obj2, &t);
        }
    }

    func_ov006_020c70d0();
    func_ov006_020c425c();

    return 1;
}
