//cpp
typedef int Fix12;
typedef unsigned char u8;
struct OamAttr { unsigned short attr0, attr1, attr2, attr3; };
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, OamAttr*, int, int, int, int, int, int, int, int);
extern "C" int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int, OamAttr*, int, int, int, int, int, int);
extern "C" int func_0203da9c(void);
extern "C" void func_ov075_0211621c(char *sl);
extern "C" int func_0200f0bc(void);
extern "C" void func_0202048c(void *dst, int a, int b);
extern u8 data_0209b2f0[];
extern OamAttr data_ov075_0211cb84;
extern OamAttr *data_ov075_0211c9f4[];
extern OamAttr *data_ov075_0211c9e0[];

#define LADR(p) ((int *)(unsigned int)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

extern "C" void func_ov075_021184b0(char *sl)
{
    int id = func_0203da9c();
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(false, &data_ov075_0211cb84, 0x80, 0x20, -1, -1, 0x1000, 0x1000, 0, -1);
    func_ov075_0211621c(sl);

    u8 *p = data_0209b2f0 + id;
    OamAttr *sb;
    if (*p != 0)
        sb = data_ov075_0211c9f4[func_0200f0bc()];
    else
        sb = data_ov075_0211c9e0[func_0200f0bc()];

    {
        int i = 0;
        int x = 0x80;
        int y = 0x60;
        do {
            int fix = (i == *(int *)(sl + 0x26c)) ? 0xe00 : 0x1000;
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(true, sb, x, y, -1, -1, fix, 0);
            if (sb->attr3 == 0xffff)
                break;
            sb++;
            i++;
        } while (1);

        {
            int *q = LADR(sl + 0x268);
            *q = *q + 1;
            if (*(int *)(sl + 0x268) >= 3) {
                *(int *)(sl + 0x268) = 0;
                {
                    int *r = LADR(sl + 0x26c);
                    *r = *r + 1;
                    if (*(int *)(sl + 0x26c) > i)
                        *(int *)(sl + 0x26c) = 0;
                }
            }
        }
    }

    if (*p != 0) {
        func_0202048c(sl + 0x20c, 0x80, 0x88);
        func_0202048c(sl + 0x1b4, 0x80, 0x88);
    } else {
        func_0202048c(sl + 0x1b4, 0x80, 0x88);
        func_0202048c(sl + 0x238, 0x6c, 0x6e);
        func_0202048c(sl + 0x20c, 0xa6, 0x6e);
    }
    func_0202048c(sl + 0x1e0, 0x80, 0x88);
}
