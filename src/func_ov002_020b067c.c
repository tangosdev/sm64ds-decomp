// @symbol func_ov002_020b067c
// recovered name: daBar_c_InitResources
/* recovered: renamed to Class_Method */
/* daBar_c::InitResources - recovered from vtable slot identity */
extern void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(char* c, int a, int b, int d, int e);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(char* thiz, char* actor, int b, int d, unsigned int e, unsigned int f);

int func_ov002_020b067c(char* c)
{
    int r5 = (((*(int*)(c + 8) & 0xff) - 0xa) * 0xa) << 0xc;
    int half;
    if (r5 <= 0) r5 = 0x1000;
    half = r5 >> 1;
    _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(c, half, half, half + 0x640000, 0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
        c + 0xd4, c, 0x35555, r5,
        (*(int*)(c + 8) & 0x100) ? 0x280000c : 0x80000c, 0);
    return 1;
}
