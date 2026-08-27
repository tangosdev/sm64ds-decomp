//cpp
extern "C" {
struct OamAttr;
extern int data_ov006_0213c114[];
extern unsigned char data_ov006_0213c064[];
extern int data_ov006_0213c074[];
}
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
    int show, struct OamAttr* attr, int a, int b, int c, int d, int e, int f);

extern "C" void func_ov006_020dec88(char* t)
{
    int idx = *(signed char*)(t + 0x12) + *(unsigned char*)(t + 0x13) * 6;
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
        1,
        (struct OamAttr*)data_ov006_0213c074[data_ov006_0213c064[idx]],
        *(int*)t >> 12,
        *(int*)(t + 4) >> 12,
        -1,
        -1,
        data_ov006_0213c114[idx],
        0);
}
