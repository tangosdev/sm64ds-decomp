//cpp
typedef int Fix12;
struct OamAttr;
struct OAM {
    static int Render(bool, OamAttr*, int, int, int, int, Fix12, OamAttr*, int, int);
    static void RenderSub(OamAttr*, int, int);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(bool, OamAttr*, int, int, int, int, Fix12, OamAttr*, int, int);

extern "C" int func_0200f0bc(void);
extern int* data_ov075_0211c9cc[];
extern short data_ov075_0211b5d4[];
extern "C" char data_ov075_0211c790[];

extern "C" void func_ov075_02117590(char* c)
{
    int* tbl = data_ov075_0211c9cc[func_0200f0bc()];
    OamAttr* attr = (OamAttr*)tbl[*(int*)(c + 0x268)];
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(false, attr, 0x80, 0x60, -1, -1, 0x1000, (OamAttr*)0x1000, 0, -1);

    int a1, a2;
    unsigned char idx = *(unsigned char*)(c + 0x281);
    if (idx < 3) {
        a2 = (short)(idx * 0x28 + 0x30);
        a1 = 0x26;
    } else {
        int j = func_0200f0bc();
        a1 = data_ov075_0211b5d4[j];
        a2 = 0xa8;
    }
    OAM::RenderSub((OamAttr*)data_ov075_0211c790, a1, a2);
}
