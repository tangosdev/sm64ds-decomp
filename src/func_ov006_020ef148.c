typedef unsigned short u16;

extern void func_ov004_020b1a5c(int a0, int a1);
extern void func_ov006_020eef58(void);
extern void func_ov006_020ef2b8(void);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void func_02045e44(void* self, unsigned int value, int index);
extern void func_ov006_020c70d0(void);
extern void func_ov006_020c425c(void);
extern int func_ov004_020ad674(void);
extern void func_ov004_020afcf8(void* a0, void* a1, int a2, void* a3);
extern void func_ov004_020afa20(int a0, int a1, int a2, int a3, int a4);

extern int data_ov006_02140308;
extern int data_020a0e68;
extern int data_ov006_0213ccb0;
extern int data_ov006_02140428;
extern int data_ov006_02134cf8;
extern int* data_ov006_0213cccc[];

struct M48 { int w[12]; };
struct S3 { int a, b, c; };

int func_ov006_020ef148(char* self)
{
    struct S3 local;
    void* p;
    int count;
    int a1v;
    int i;

    func_ov004_020b1a5c(data_ov006_02140308, 4);
    func_ov006_020eef58();
    func_ov006_020ef2b8();
    Matrix4x3_FromTranslation(&data_020a0e68, 0, 0xffead000, -0xa0000);
    *(struct M48*)(self + 0x5a30) = *(struct M48*)&data_020a0e68;

    local = *(struct S3*)&data_ov006_0213ccb0;

    {
        int* obj = (int*)(self + 0x5a1c);
        p = (void*)obj[1];
        func_02045e44((void*)obj, 1, 0);
    }
    *(int*)((char*)p + 0x18) = *(int*)(self + 0x5a64);
    {
        void** vobj = (void**)(((long long)(int)(self + 0x5a14)) & 0xFFFFFFFFFFFFFFFFLL);
        (*(void(**)(void*, void*))((char*)*vobj + 0x14))((void*)vobj, &local);
    }

    func_ov006_020c70d0();
    func_ov006_020c425c();

    if (*(u16*)(self + 0x4664) == 1) {
        a1v = 0x6e;
        count = data_ov006_02140428;
        for (i = 0; i < 3; i++) {
            if (i >= count) {
                int idx = func_ov004_020ad674();
                func_ov004_020afcf8((void*)data_ov006_0213cccc[idx][1], (void*)a1v, 0xc, (void*)0);
            } else {
                func_ov004_020afa20(data_ov006_02134cf8, a1v, 0xc, -1, -1);
            }
            a1v += 0x12;
        }
    }

    return 1;
}
