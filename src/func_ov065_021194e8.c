extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *m, void *f, int a, int b);
extern void func_ov001_020ab228(void *a, void *b, int c, int d, int e);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *clsn, void *actor, int a, int b, unsigned int c, unsigned int d);
struct G { int w[2]; };
extern struct G WARIO_CAP_MODEL_PTR;

int func_ov065_021194e8(char *c)
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xf0, (void *)WARIO_CAP_MODEL_PTR.w[1], 1, -1);
    func_ov001_020ab228(c + 0xd4, c, 2, 2, 0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x140, c, 0x96000, 0x96000, 0x100002, 0x8000);
    *(int *)(c + 0x174) = 0;
    return 1;
}
