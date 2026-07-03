//cpp
extern "C" {
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void *self, void *file, int a, int b, int c, unsigned short d);
extern short _ZN5Actor18HorzAngleToCPlayerEv(void *self);
extern void *data_ov073_021232a8[];

int func_ov073_02120d80(char *c)
{
    int fix;
    unsigned short t;
    short ang;

    fix = 0x1000;
    t = 0;
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x30c, data_ov073_021232a8[1], 4, 0x40000000, fix, t);
    *(int *)(c + 0x368) = fix;
    ang = _ZN5Actor18HorzAngleToCPlayerEv(c);
    *(short *)(c + 0x94) = ang;
    *(short *)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFF) =
        (short)((int)*(short *)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFF) + 0x8000);
    return 1;
}
}