//cpp
extern "C" {
struct OamAttr;
extern char* data_ov004_020beb68;
}
typedef int Fix12i;
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
    int show, struct OamAttr* attr, int a, int b, int c, int d, Fix12i e, int f);

extern "C" int func_ov004_020af770(int a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6)
{
    char* g = data_ov004_020beb68;
    if (g == 0) return 0;
    if (*(int*)(g + 0x4628) == 0) {
        int (*vf)(char*) = *(int(**)(char*))(*(char**)g + 0x68);
        if (vf(g) == 2) {
            if (*(unsigned short*)(data_ov004_020beb68 + 0x4664) != 0) return 0;
            return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, (struct OamAttr*)a0, a1, a2, a3, a4, a5, a6);
        }
    }
    return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, (struct OamAttr*)a0, a1, a2, a3, a4, a5, a6);
}
