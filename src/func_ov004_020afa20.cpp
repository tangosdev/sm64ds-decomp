//cpp
extern "C" {
struct OamAttr;
extern char* data_ov004_020beb68;
}
typedef int Fix12i;
extern "C" void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    int show, struct OamAttr* attr, int a, int b, int c, int d,
    Fix12i e, Fix12i f, int g, int h);

extern "C" void func_ov004_020afa20(int a0, int a1, int a2, int a3, int a4)
{
    char* g = data_ov004_020beb68;
    if (g == 0) return;
    if (*(int*)(g + 0x4628) == 0) {
        int (*vf)(char*) = *(int(**)(char*))(*(char**)g + 0x68);
        if (vf(g) == 2) {
            if (*(unsigned short*)(data_ov004_020beb68 + 0x4664) != 1) return;
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (struct OamAttr*)a0, a1, a2, a3, a4, 0x1000, 0x1000, 0, -1);
            return;
        }
    }
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, (struct OamAttr*)a0, a1, a2, a3, a4, 0x1000, 0x1000, 0, -1);
}
