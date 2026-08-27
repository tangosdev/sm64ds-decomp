//cpp
typedef int Fix12;
extern "C" {
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned a, unsigned b, unsigned c, Fix12 d, int e);
int func_ov002_020f93a8(char* c, unsigned short* p) {
    if (p[0] < *(unsigned short*)(c+0xde)) {
        int d8 = *(int*)(c+0xd8);
        if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(*(int*)(c+0xd4), d8, 0x7f, ((0x7f - d8) << 12) / 5, 1) != 0)
            *(unsigned short*)(c+0xde) = 1;
        p[0]++;
    } else {
        if (*(unsigned short*)(c+0xde) == 0) {
            if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(*(int*)(c+0xd4), 0x7f, 0, 0xcb33, 1) != 0)
                return 1;
        }
    }
    return 0;
}
}
