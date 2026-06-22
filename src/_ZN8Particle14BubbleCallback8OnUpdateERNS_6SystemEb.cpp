//cpp
typedef int Fix12i;
typedef unsigned short u16;

struct Node {
    Node *next;       /* 0x00 */
    char pad4[4];
    int f8;           /* 0x08 */
    int fc;           /* 0x0c */
    int f10;          /* 0x10 */
    int f14;          /* 0x14 */
    int f18;          /* 0x18 */
    int f1c;          /* 0x1c */
    char pad20[0x2c-0x20];
    u16 f2c;          /* 0x2c */
    u16 f2e;          /* 0x2e */
};

extern "C" int data_0209f32c;
extern "C" unsigned int _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_(Fix12i x, Fix12i y, Fix12i z);
extern "C" int _ZN8Particle14SimpleCallback8OnUpdateERNS_6SystemEb(char *self, char *sys, int b);

extern "C" int _ZN8Particle14BubbleCallback8OnUpdateERNS_6SystemEb(char *self, char *sys, int b)
{
    Node *node = *(Node **)(sys + 8);
    while (node != 0) {
        int test = node->f18 + node->fc;
        int xx = node->f14 + node->f8;
        int zz = node->f1c + node->f10;
        if ((test << 3) > data_0209f32c) {
            node->f2e = node->f2c;
            _ZN8Particle6System9NewRippleE5Fix12IiES2_S2_(
                xx << 3,
                data_0209f32c + 0x3000,
                zz << 3);
        }
        node = node->next;
    }
    return _ZN8Particle14SimpleCallback8OnUpdateERNS_6SystemEb(self, sys, b);
}
