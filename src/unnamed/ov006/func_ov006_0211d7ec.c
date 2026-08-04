typedef int s32;
typedef int Fix12i;

struct OamAttri { unsigned short attr0, attr1, attr2, attr3; };

extern struct OamAttri *data_ov006_0213a628[];

extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    int sub, struct OamAttri *data,
    s32 x, s32 y,
    s32 palette, s32 priority,
    Fix12i scaleX, Fix12i scaleY,
    s32 rotation, s32 mode);

void func_ov006_0211d7ec(char *c)
{
    if (*(unsigned char *)(c + 0x4bb9) == 0) return;
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
        0,
        data_ov006_0213a628[*(unsigned char *)(c + 0x4bb8)],
        *(int *)(c + 0x4ba0) >> 12,
        *(int *)(c + 0x4ba4) >> 12,
        -1, 0,
        0x1000, 0x1000,
        *(unsigned short *)(c + 0x4bb0), -1);
}
