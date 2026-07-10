extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *m, void *f, int a, int b);
extern void func_ov001_020ab228(void *a, void *b, int c, int d, int e);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *clsn, void *actor, int a, int b, unsigned int c, unsigned int d);
struct G { int w[2]; };
extern struct G data_ov002_0210d9c0;

int _ZN9DorrieCap13InitResourcesEv(char *c)
{
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xf0, (void *)data_ov002_0210d9c0.w[1], 1, -1);
    func_ov001_020ab228(c + 0xd4, c, 2, 2, 0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x140, c, 0x96000, 0x96000, 0x100002, 0x8000);
    *(int *)(c + 0x174) = 0;
    return 1;
}
