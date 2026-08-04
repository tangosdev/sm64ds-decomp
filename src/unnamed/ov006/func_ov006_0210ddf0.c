typedef int s32;
typedef int Fix12i;

struct OamAttri { unsigned short attr0, attr1, attr2, attr3; };

extern void *data_ov006_02137994[];
extern struct OamAttri *data_ov006_02137684[];

extern void func_ov004_020afdd0(void *a0, int a1, int a2, int a3, int a4);

extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    int sub, struct OamAttri *data,
    s32 x, s32 y,
    s32 palette, s32 priority,
    Fix12i scaleX, Fix12i scaleY,
    s32 rotation, s32 mode);

void func_ov006_0210ddf0(char *c)
{
    if (*(int *)(c + 0x40) <= 1) {
        func_ov004_020afdd0(data_ov006_02137994[0],
            *(int *)(c + 8) >> 12, *(int *)(c + 0xc) >> 12, -1, 1);
        func_ov004_020afdd0(data_ov006_02137994[1],
            *(int *)(c + 8) >> 12, (*(int *)(c + 0xc) >> 12) + 5, -1, 2);
    }
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0, data_ov006_02137684[0],
        (*(int *)(c + 8) >> 12) - 0x10, (*(int *)(c + 0xc) >> 12) + 0x118,
        -1, 1, 0x1000, 0x1000,
        (unsigned short)(short)(-0x4000 - *(short *)(c + 0x32)), -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0, data_ov006_02137684[0],
        (*(int *)(c + 8) >> 12) + 0x10, (*(int *)(c + 0xc) >> 12) + 0x118,
        -1, 1, 0x1000, 0x1000,
        (unsigned short)(short)(*(short *)(c + 0x32) - 0x4000), -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0, data_ov006_02137684[1],
        (*(int *)(c + 8) >> 12) - 0x10, (*(int *)(c + 0xc) >> 12) + 0x11d,
        -1, 2, 0x1000, 0x1000,
        (unsigned short)(short)(-0x4000 - *(short *)(c + 0x32)), -1);
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0, data_ov006_02137684[1],
        (*(int *)(c + 8) >> 12) + 0x10, (*(int *)(c + 0xc) >> 12) + 0x11d,
        -1, 2, 0x1000, 0x1000,
        (unsigned short)(short)(*(short *)(c + 0x32) - 0x4000), -1);
}
