//cpp
// func_ov004_020aff38 at 0x020aff38
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov004).
extern "C" {
struct OamAttr;
extern char* data_ov004_020beb68;
extern int data_ov004_020beb6c;
}
typedef int Fix12i;
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
    int show, struct OamAttr* attr, int a, int b, int c, int d, Fix12i e, int f);

extern "C" int func_ov004_020aff38(struct OamAttr* a0, int a1, int a2, int a3, int a4, Fix12i a5, int a6)
{
    char* g = data_ov004_020beb68;
    if (g == 0) return 0;
    if (*(int*)(g + 0x4628) == 0 && (*(int(**)(char*))(*(char**)g + 0x68))(g) == 2) {
        if (*(unsigned short*)(data_ov004_020beb68 + 0x4664) == 1) {
            int hp = data_ov004_020beb6c;
            if (a2 >= -0x100 - hp && a2 < -hp) {
                return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, a0, a1, a2 + 0xc0 + hp, a3, a4, a5, a6);
            }
            if (a2 >= -0x40 && a2 < 0xc0) {
                return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, a0, a1, a2, a3, a4, a5, a6);
            }
        }
    } else {
        int hp = data_ov004_020beb6c;
        if (a2 >= -0x100 - hp && a2 < -hp) {
            return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(0, a0, a1, a2 + 0xc0 + hp, a3, a4, a5, a6);
        }
        if (a2 >= -0x40 && a2 < 0xc0) {
            return _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, a0, a1, a2, a3, a4, a5, a6);
        }
    }
    return 0;
}
