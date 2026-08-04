typedef int Fix12;
extern void* _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12 x, Fix12 y, Fix12 z);
extern void _Z14ApproachLinearRiii(int* p, int a, int b);
extern void func_ov006_020bfff8(void* a, void* b, int* c, int* d);
extern int func_ov004_020b04c0(void);
extern void func_ov006_020ef05c(int a, int b, int c);
extern void func_ov006_020c4d20(char* p);

extern int data_ov006_02140304[];
extern int data_ov006_02140308;
extern int data_ov006_02140314;
extern void* data_ov006_02141a50;
extern void* data_ov006_02141a40;

void func_ov006_020c4d3c(char* c)
{
    void* a;
    void* b;
    int v0, v1;
    int t;

    a = _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x122,
        *(int*)(c + 0x9c) << 3, *(int*)(c + 0xa0) << 3, *(int*)(c + 0xa4) << 3);
    b = _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x123,
        *(int*)(c + 0x9c) << 3, *(int*)(c + 0xa0) << 3, *(int*)(c + 0xa4) << 3);
    t = (int)(*(int*)(c + 0xd8) << 0x1c) >> 0x10;
    if (a) *(int*)((char*)a + 0x50) = t;
    if (b) *(int*)((char*)a + 0x50) = t;

    _Z14ApproachLinearRiii(data_ov006_02140304, 0, 1);
    _Z14ApproachLinearRiii(&data_ov006_02140308, 0x270f, 1);

    if (data_ov006_02140314 != 0) {
        if (*(int*)(c + 0xa0) > 0) {
            func_ov006_020bfff8(data_ov006_02141a50, (void*)(c + 0x9c), &v0, &v1);
            v1 = v1 - (func_ov004_020b04c0() + 0xc0);
        } else {
            func_ov006_020bfff8(data_ov006_02141a40, (void*)(c + 0x9c), &v0, &v1);
        }
        v1 = v1 - 0x20;
        func_ov006_020ef05c(v0 << 0xc, v1 << 0xc, (short)data_ov006_02140308);
    }

    func_ov006_020c4d20(c);
}
